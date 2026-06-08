"""
Unit tests for the Direction enum.
"""
from ia.shared.enum import Direction

def test_direction_values():
    """Test the values of the Direction enum."""
    assert Direction.NORTH.value == 1
    assert Direction.EAST.value == 2
    assert Direction.SOUTH.value == 3
    assert Direction.WEST.value == 4
