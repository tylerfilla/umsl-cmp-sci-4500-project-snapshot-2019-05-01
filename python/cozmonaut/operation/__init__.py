#
# Cozmonaut
# Copyright 2019 The Cozmonaut Contributors
#

from abc import ABC, abstractmethod


class AbstractOperation(ABC):
    """
    An operation.
    """

    @abstractmethod
    def start(self):
        """
        Start the operation.
        """

    @abstractmethod
    def stop(self):
        """
        Stop the operation.
        """
