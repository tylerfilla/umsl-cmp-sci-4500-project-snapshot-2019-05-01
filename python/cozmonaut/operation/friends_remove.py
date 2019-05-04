#
# Cozmonaut
# Copyright 2019 The Cozmonaut Contributors
#

from cozmonaut.operation import AbstractOperation


class OperationFriendsRemove(AbstractOperation):
    """
    The friends remove operation.

    In this mode, we remove friends as requested.
    """

    def __init__(self, args: dict):
        print('friends remove init')

    def start(self):
        print('friends remove start')

    def stop(self):
        print('friends remove stop')
