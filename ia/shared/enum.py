"""Shared enumerations for the Zappy bot."""
from enum import Enum


class Direction(Enum):
    """Enumeration of possible directions for the bot."""
    NORTH = 1
    EAST = 2
    SOUTH = 3
    WEST = 4


class Move(Enum):
    """Movement commands accepted by the Zappy server."""
    FORWARD = "Forward"
    RIGHT = "Right"
    LEFT = "Left"


class State(Enum):
    """High-level behaviour states for the bot state machine."""
    SURVIVAL = "survival"
    EATING = "eating"


class Resource(Enum):
    """Resources found on the Zappy map."""
    FOOD = "food"
    LINEMATE = "linemate"
    DERAUMERE = "deraumere"
    SIBUR = "sibur"
    MENDIANE = "mendiane"
    PHIRAS = "phiras"
    THYSTAME = "thystame"
