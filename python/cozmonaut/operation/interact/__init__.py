#
# Cozmonaut
# Copyright 2019 The Cozmonaut Contributors
#

import asyncio
import threading
from enum import Enum

import cozmo

from cozmonaut.operation import AbstractOperation
from cozmonaut.operation.interact.face_tracker import FaceTracker


class OperationInteractMode(Enum):
    """
    A mode for interaction with Cozmo(s).
    """

    both = 0  # Run both Cozmos
    only_a = 1  # Only run Cozmo A
    only_b = 2  # Only run Cozmo B


class OperationInteract(AbstractOperation):
    """
    The interactive mode operation.

    In this mode, the Cozmo robots are driven around to perform the primary goal
    of meeting and greeting people. Support is hardcoded for two Cozmo robots,
    and they are assigned the roles of Cozmo A and Cozmo B.

    TODO: Add information about how they interact with passersby and themselves
    """

    def __init__(self, args: dict):
        # Grab mode from args while defaulting to mode 'both'
        self._mode: OperationInteractMode = OperationInteractMode[args.get('mode', 'both')]

        # Grab A and B serial numbers from args
        self._serial_a: str = args.get('serial_a', '')
        self._serial_b: str = args.get('serial_b', '')

        # Kill switch for the interact loop
        # When set, we are about to begin the cleanup process
        self._should_stop = False

        # Shutdown status for the interact loop
        # When set, we are in the process of cleaning up
        self._stopping = False

        # The interact thread
        self._thread = None

        # The face trackers
        # Each Cozmo robot gets its own
        self._face_tracker_a = FaceTracker()
        self._face_tracker_b = FaceTracker()

        # The live robot instances
        self._robot_a = None
        self._robot_b = None

    def start(self):
        # Clear the kill switch and cleanup state
        self._should_stop = False
        self._stopping = False

        # Start operation thread
        self._thread = threading.Thread(target=self._thread_main)
        self._thread.start()

    def stop(self):
        # Set the kill switch
        # The watchdog will kick in soon enough and start cleanup
        self._should_stop = True

        # Wait for the thread to die
        self._thread.join()
        self._thread = None

    def on_wasd_set_fwd(self):
        """
        Called (C -> Python) on WASD forward set.

        Usually, this gets called when the W key goes down.
        """

        print('WASD Set Forward')

    def on_wasd_clr_fwd(self):
        """
        Called (C -> Python) on WASD forward cleared.

        Usually, this gets called when the W key goes up.
        """

        print('WASD Clear Forward')

    def on_wasd_set_rev(self):
        """
        Called (C -> Python) on WASD reverse set.

        Usually, this gets called when the S key goes down.
        """

        print('WASD Set Reverse')

    def on_wasd_clr_rev(self):
        """
        Called (C -> Python) on WASD reverse cleared.

        Usually, this gets called when the S key goes up.
        """

        print('WASD Clear Reverse')

    def on_wasd_set_left(self):
        """
        Called (C -> Python) on WASD left set.

        Usually, this gets called when the A key goes down.
        """

        print('WASD Set Left')

    def on_wasd_clr_left(self):
        """
        Called (C -> Python) on WASD left cleared.

        Usually, this gets called when the A key goes up.
        """

        print('WASD Clear Left')

    def on_wasd_set_right(self):
        """
        Called (C -> Python) on WASD right set.

        Usually, this gets called when the D key goes down.
        """

        print('WASD Set Right')

    def on_wasd_clr_right(self):
        """
        Called (C -> Python) on WASD right cleared.

        Usually, this gets called when the D key goes up.
        """

        print('WASD Clear Right')

    def _thread_main(self):
        # Create an event loop on this thread
        loop = asyncio.new_event_loop()

        print(f'Want Cozmo A to have serial number {self._serial_a or "(unknown)"}')
        print(f'Want Cozmo B to have serial number {self._serial_b or "(unknown)"}')

        while True:
            # Connect to next available Cozmo
            try:
                conn = cozmo.connect_on_loop(loop)
            except cozmo.exceptions.NoDevicesFound:
                break

            # Wait for the robot to become available
            # We must do this to read its serial number
            robot = loop.run_until_complete(conn.wait_for_robot())

            print(f'Found a robot with serial {robot.serial}')

            # Keep robot instances with desired serial numbers
            if robot.serial == self._serial_a:
                self._robot_a = robot
            if robot.serial == self._serial_b:
                self._robot_b = robot

            # If both are assigned, we're good!
            if self._robot_a is not None and self._robot_b is not None:
                print('Both Cozmo A and Cozmo B assigned')
                break

        # A list for main function coroutines
        # Coroutine objects for both the _cozmo_a_main and _cozmo_b_main async functions can go here
        # Or neither of them can go here (that depends on which serial numbers were specified and found above)
        coroutines_for_cozmo = []

        # If we assigned a robot instance to play Cozmo A...
        if self._robot_a is not None:
            print(f'The role of Cozmo A is being played by robot {self._robot_a.serial}')

            # Obtain a coroutine for Cozmo A main function
            # Add the coroutine to the above coroutine list
            coroutines_for_cozmo.append(self._cozmo_a_main(self._robot_a))
        else:
            print('Unable to cast the role of Cozmo A')

            # If the current mode requires Cozmo A to be assigned
            # The two modes that require this are "only_a" and "both"
            if self._mode == OperationInteractMode.only_a or self._mode == OperationInteractMode.both:
                print('Refusing to continue because Cozmo A was not assigned')
                return
            else:
                print('Continuing without Cozmo A...')

        # If we assigned a robot instance to play Cozmo B...
        if self._robot_b is not None:
            print(f'The role of Cozmo B is being played by robot {self._robot_b.serial}')

            # Obtain a coroutine for Cozmo B main function
            # Add the coroutine to the above coroutine list
            coroutines_for_cozmo.append(self._cozmo_b_main(self._robot_b))
        else:
            print('Unable to cast the role of Cozmo B')

            # If the current mode requires Cozmo B to be assigned
            # The two modes that require this are "only_b" and "both"
            if self._mode == OperationInteractMode.only_b or self._mode == OperationInteractMode.both:
                print('Refusing to continue because Cozmo B was not assigned')
                return
            else:
                print('Continuing without Cozmo B...')

        # This wraps everything into one task object and schedules it on the loop
        asyncio.gather(
            # The operation watchdog (tells us when to call it quits)
            self._watchdog(),

            # Expand the main coroutines list into arguments
            *coroutines_for_cozmo,

            # Use the new loop
            loop=loop,
        )

    async def _cozmo_a_main(self, robot: cozmo.robot.Robot):
        """
        Main function for Cozmo A.

        :param robot: Cozmo A instance
        """

        # Register to receive camera frames from this robot
        robot.camera.add_event_handler(cozmo.robot.camera.EvtNewRawCameraImage, self._cozmo_a_on_new_raw_camera_image)

        # Schedule a battery watcher for this robot onto the loop
        coro_batt = asyncio.ensure_future(self._battery_watcher(robot))

        # Schedule a face watcher for this robot onto the loop
        coro_face = asyncio.ensure_future(self._face_watcher(robot))

        # Loop for Cozmo A
        while not self._stopping:
            # TODO: Swap back and forth between active and idle

            # Yield control to other coroutines
            await asyncio.sleep(0)

        # Wait for face coroutine to stop
        await coro_face

        # Wait for battery coroutine to stop
        await coro_batt

    def _cozmo_a_on_new_raw_camera_image(self, evt: cozmo.robot.camera.EvtNewRawCameraImage, **kwargs):
        """
        Event handler for Cozmo A's raw camera image event.

        This function is not asynchronous, so go fast!

        :param evt: The event instance
        """

        # TODO: Send the image off to face tracker A

    async def _cozmo_b_main(self, robot: cozmo.robot.Robot):
        """
        Main function for Cozmo B.

        :param robot: Cozmo B instance
        """

        # Register to receive camera frames from this robot
        robot.camera.add_event_handler(cozmo.robot.camera.EvtNewRawCameraImage, self._cozmo_b_on_new_raw_camera_image)

        # Schedule a battery watcher for this robot onto the loop
        coro_batt = asyncio.ensure_future(self._battery_watcher(robot))

        # Schedule a face watcher for this robot onto the loop
        coro_face = asyncio.ensure_future(self._face_watcher(robot))

        # Loop for Cozmo B
        while not self._stopping:
            # TODO: Swap back and forth between active and idle (but opposite that of Cozmo A)

            # Yield control to other coroutines
            await asyncio.sleep(0)

        # Wait for face coroutine to stop
        await coro_face

        # Wait for battery coroutine to stop
        await coro_batt

    def _cozmo_b_on_new_raw_camera_image(self, evt: cozmo.robot.camera.EvtNewRawCameraImage, **kwargs):
        """
        Event handler for Cozmo B's raw camera image event.

        This function is not asynchronous, so go fast!

        :param evt: The event instance
        """

        # TODO: Send the image off to face tracker B

    async def _watchdog(self):
        """
        The watchdog process.

        This looks out for the "kill switch" set by another thread. If it is
        set, we safe the robots and clean up gracefully.
        """

        while not self._stopping:
            # If the kill switch is set but we're not stopping yet
            if self._should_stop and not self._stopping:
                # We're stopping now
                self._stopping = True

                # TODO: Return to charger

                # Politely ask the loop to stop
                loop = asyncio.get_event_loop()
                loop.call_soon(loop.stop)

            # Yield control to other coroutines
            await asyncio.sleep(0)

    async def _battery_watcher(self, robot: cozmo.robot.Robot):
        """
        A battery watcher for a Cozmo robot.

        This is responsible for watching the battery potential on a robot object
        and returning the robot to the charger.

        :param robot: The robot instance
        """

        while not self._stopping:
            # If battery potential is below the recommended "low" level
            if robot.battery_voltage < 3.5:
                # TODO: Return to charger
                break

            # Yield control to other coroutines
            await asyncio.sleep(0)

    async def _face_watcher(self, robot: cozmo.robot.Robot):
        """
        A face watcher for a Cozmo robot.

        :param robot: The robot instance
        """

        # Enable color imaging on this robot's camera
        robot.camera.color_image_enabled = True
        robot.camera.image_stream_enabled = True

        # Pick the face tracker for this robot
        # Cozmo A gets tracker A and Cozmo B gets tracker B
        ft = None
        if robot == self._robot_a:
            ft = self._face_tracker_a
        elif robot == self._robot_b:
            ft = self._face_tracker_b

        while not self._stopping:
            # Wait for the next face tracked by the tracker
            track = await asyncio.wrap_future(ft.next_track())

            # TODO: Make Cozmo look at the face for social cue
            #   Hopefully we don't lose the track b/c motion blur, but I think I know a hack if we do

            await asyncio.sleep(0.25)  # TODO: Let motion blur settle down? Am I overestimating motion blur?

            # Request to recognize the face
            rec = await asyncio.wrap_future(ft.recognize(track.index))

            # TODO: Greet the face if rec.fid is not negative one
            #  If rec.fid is negative one, then meet the new person and store a Base64 copy of rec.ident to the DB
            #  Don't forget to then add it to the trackers with tracker_a.add_identity and tracker_b.add_identity

            # Yield control to other coroutines
            await asyncio.sleep(0)


# Do not leave the charger until we say it's okay
cozmo.robot.Robot.drive_off_charger_on_connect = False