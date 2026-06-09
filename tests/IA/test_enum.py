"""Unit tests for the shared enums."""
from ia.shared.enum import Direction, Move, Resource


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


def test_resource_contains_food():
    """
    Given the Resource enum
    When it is inspected
    Then it contains a FOOD member with value "food"
    """
    assert Resource.FOOD.value == "food"


def test_resource_contains_all_minerals():
    """
    Given the Resource enum
    When its values are inspected
    Then it contains the 6 mineral resources from the Zappy protocol
    """
    mineral_values = {"linemate", "deraumere", "sibur", "mendiane", "phiras", "thystame"}
    assert mineral_values.issubset({r.value for r in Resource})


def test_resource_has_seven_members():
    """
    Given the Resource enum
    When its length is checked
    Then it has exactly 7 members (food + 6 minerals)
    """
    assert len(Resource) == 7
