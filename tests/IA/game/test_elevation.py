"""Unit tests for elevation requirements table and helpers."""
import pytest
from ia.game.elevation import ELEVATION_REQUIREMENTS, stones_missing, is_ready_to_elevate


STONES = ["linemate", "deraumere", "sibur", "mendiane", "phiras", "thystame"]


@pytest.mark.parametrize("level", range(1, 8))
def test_elevation_requirements_has_all_levels(level):
    """
    Given the ELEVATION_REQUIREMENTS table
    When accessing a level between 1 and 7
    Then the entry exists and contains players and all six stones
    """
    assert level in ELEVATION_REQUIREMENTS
    req = ELEVATION_REQUIREMENTS[level]
    assert "players" in req
    for stone in STONES:
        assert stone in req


def test_elevation_requirements_level1():
    """
    Given the ELEVATION_REQUIREMENTS table
    When reading level 1
    Then it requires 1 player and 1 linemate only
    """
    req = ELEVATION_REQUIREMENTS[1]
    assert req["players"] == 1
    assert req["linemate"] == 1
    for stone in STONES[1:]:
        assert req[stone] == 0


def test_elevation_requirements_level7():
    """
    Given the ELEVATION_REQUIREMENTS table
    When reading level 7
    Then it requires 6 players and specific stone quantities including 1 thystame
    """
    req = ELEVATION_REQUIREMENTS[7]
    assert req["players"] == 6
    assert req["linemate"] == 2
    assert req["deraumere"] == 2
    assert req["sibur"] == 2
    assert req["mendiane"] == 2
    assert req["phiras"] == 2
    assert req["thystame"] == 1


def test_stones_missing_returns_empty_when_inventory_is_sufficient():
    """
    Given a level-1 requirement and an inventory containing 1 linemate
    When stones_missing is called
    Then it returns an empty dict
    """
    inventory = {"linemate": 1}
    assert not stones_missing(1, inventory)


def test_stones_missing_returns_deficit_for_each_short_stone():
    """
    Given a level-2 requirement and an empty inventory
    When stones_missing is called
    Then it returns all required stones with their full amounts
    """
    result = stones_missing(2, {})
    assert result["linemate"] == 1
    assert result["deraumere"] == 1
    assert result["sibur"] == 1


def test_stones_missing_partial_inventory():
    """
    Given a level-3 requirement and an inventory with 2 linemate already collected
    When stones_missing is called
    Then linemate is absent from the result and only the remaining stones appear
    """
    inventory = {"linemate": 2}
    result = stones_missing(3, inventory)
    assert "linemate" not in result
    assert result["sibur"] == 1
    assert result["phiras"] == 2


def test_stones_missing_does_not_include_players():
    """
    Given any level and any inventory
    When stones_missing is called
    Then the returned dict does not contain the 'players' key
    """
    assert "players" not in stones_missing(1, {})
    assert "players" not in stones_missing(7, {})


@pytest.mark.parametrize("level", range(1, 8))
def test_stones_missing_full_inventory_returns_empty(level):
    """
    Given a level and an inventory that exactly meets all requirements
    When stones_missing is called
    Then it returns an empty dict
    """
    inventory = {k: v for k, v in ELEVATION_REQUIREMENTS[level].items() if k != "players"}
    assert not stones_missing(level, inventory)


def test_is_ready_to_elevate_true_when_all_stones_present():
    """
    Given a level-1 requirement and an inventory with 1 linemate
    When is_ready_to_elevate is called
    Then it returns True
    """
    assert is_ready_to_elevate(1, {"linemate": 1}) is True


def test_is_ready_to_elevate_false_when_a_stone_is_missing():
    """
    Given a level-2 requirement and an inventory missing deraumere
    When is_ready_to_elevate is called
    Then it returns False
    """
    inventory = {"linemate": 1, "sibur": 1}
    assert is_ready_to_elevate(2, inventory) is False


def test_is_ready_to_elevate_false_when_inventory_is_empty():
    """
    Given any level above 1 and an empty inventory
    When is_ready_to_elevate is called
    Then it returns False
    """
    assert is_ready_to_elevate(2, {}) is False


@pytest.mark.parametrize("level", range(1, 8))
def test_is_ready_to_elevate_true_for_each_level_with_exact_inventory(level):
    """
    Given a level and an inventory that exactly meets all stone requirements
    When is_ready_to_elevate is called
    Then it returns True
    """
    inventory = {k: v for k, v in ELEVATION_REQUIREMENTS[level].items() if k != "players"}
    assert is_ready_to_elevate(level, inventory) is True


@pytest.mark.parametrize("level", [0, 8])
def test_stones_missing_raises_for_invalid_level(level):
    """
    Given a level outside the valid range 1-7
    When stones_missing is called
    Then it raises ValueError
    """
    with pytest.raises(ValueError):
        stones_missing(level, {})


@pytest.mark.parametrize("level", [0, 8])
def test_is_ready_to_elevate_raises_for_invalid_level(level):
    """
    Given a level outside the valid range 1-7
    When is_ready_to_elevate is called
    Then it raises ValueError
    """
    with pytest.raises(ValueError):
        is_ready_to_elevate(level, {})
