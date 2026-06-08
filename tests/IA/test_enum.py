"""
Unit tests for the Direction enum.
"""
from ia.shared.enum import Direction

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
