"""
Unit tests for the shared enums.
"""
from ia.shared.enum import Direction, Move


def test_direction_values():
    """
    Given the Direction enum
    When its members are inspected
    Then NORTH/EAST/SOUTH/WEST map to 1/2/3/4
    """
    assert Direction.NORTH.value == 1
    assert Direction.EAST.value == 2
    assert Direction.SOUTH.value == 3
    assert Direction.WEST.value == 4


def test_move_values():
    """
    Given the Move enum
    When its members are inspected
    Then FORWARD/RIGHT/LEFT map to the server command strings
    """
    assert Move.FORWARD.value == "Forward"
    assert Move.RIGHT.value == "Right"
    assert Move.LEFT.value == "Left"
