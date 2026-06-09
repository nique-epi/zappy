"""Unit tests for the shared enums and RESOURCES constant."""
from ia.shared.enum import Direction, Move, RESOURCES


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


def test_resources_contains_food():
    """
    Given the RESOURCES constant
    When it is inspected
    Then it contains the food resource
    """
    assert "food" in RESOURCES


def test_resources_contains_all_seven_minerals():
    """
    Given the RESOURCES constant
    When it is inspected
    Then it contains the 6 mineral resources from the Zappy protocol
    """
    minerals = {"linemate", "deraumere", "sibur", "mendiane", "phiras", "thystame"}
    assert minerals.issubset(set(RESOURCES))


def test_resources_has_seven_entries():
    """
    Given the RESOURCES constant
    When its length is checked
    Then it has exactly 7 entries (food + 6 minerals)
    """
    assert len(RESOURCES) == 7
