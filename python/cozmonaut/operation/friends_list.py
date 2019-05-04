#
# Cozmonaut
# Copyright 2019 The Cozmonaut Contributors
#

from cozmonaut.operation import AbstractOperation


class OperationFriendsList(AbstractOperation):
    """
    The friends list operation.

    In this mode, we list details about friends as requested.
    """

    def __init__(self, args: dict):
        print('friends list init')

    def start(self):
        print('friends list start')

    def stop(self):
        print('friends list stop')
