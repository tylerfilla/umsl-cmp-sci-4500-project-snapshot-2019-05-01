#
# Cozmonaut
# Copyright 2019 The Cozmonaut Contributors
#

from cozmonaut.operation import AbstractOperation


class OperationInteract(AbstractOperation):
    """
    The interactive mode operation.

    In this mode, the Cozmo robots are driven around to perform the primary goal
    of meeting and greeting people. Support is hardcoded for two Cozmo robots,
    and they are assigned the roles of Cozmo A and Cozmo B.

    TODO: Add information about how they interact with passersby and themselves
    """

    def __init__(self, args: dict):
        print('init')

    def start(self):
        print('start')

    def stop(self):
        print('stop')
