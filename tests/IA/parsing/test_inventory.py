"""Unit tests for inventory parsing and food threshold helpers."""
from ia.parsing.inventory import parse_inventory, needs_food, needs_food_safe
from ia.shared.enum import Resource
from ia.config import FOOD_THRESHOLD, FOOD_SAFE_THRESHOLD


def test_parse_inventory_all_zeros_on_empty_response():
    """
    Given an empty string
    When parse_inventory is called
    Then every resource is initialised to 0
    """
    result = parse_inventory("")
    assert result == dict.fromkeys(Resource, 0)


def test_parse_inventory_parses_all_resources():
    """
    Given a full inventory response from the server
    When parse_inventory is called
    Then every resource value matches the server response
    """
    response = "[food 10, linemate 1, deraumere 2, sibur 3, mendiane 4, phiras 5, thystame 6]"
    result = parse_inventory(response)
    assert result[Resource.FOOD] == 10
    assert result[Resource.LINEMATE] == 1
    assert result[Resource.DERAUMERE] == 2
    assert result[Resource.SIBUR] == 3
    assert result[Resource.MENDIANE] == 4
    assert result[Resource.PHIRAS] == 5
    assert result[Resource.THYSTAME] == 6


def test_parse_inventory_unknown_resource_is_ignored():
    """
    Given a response containing an unrecognised resource name
    When parse_inventory is called
    Then the unknown resource is silently ignored
    """
    result = parse_inventory("[food 3, unknownstuff 99]")
    assert Resource.FOOD in result
    assert result[Resource.FOOD] == 3
    assert len(result) == len(Resource)


def test_parse_inventory_missing_resource_defaults_to_zero():
    """
    Given a response that omits some resources
    When parse_inventory is called
    Then the missing resources are 0
    """
    result = parse_inventory("[food 5]")
    assert result[Resource.LINEMATE] == 0
    assert result[Resource.THYSTAME] == 0


def test_parse_inventory_returns_all_resource_keys():
    """
    Given any inventory response
    When parse_inventory is called
    Then the returned dict contains exactly the Resource enum members as keys
    """
    result = parse_inventory("[food 1]")
    assert set(result.keys()) == set(Resource)


def test_parse_inventory_malformed_quantity_is_ignored():
    """
    Given a response with a non-integer quantity
    When parse_inventory is called
    Then the malformed entry is skipped and the resource stays at 0
    """
    result = parse_inventory("[food abc]")
    assert result[Resource.FOOD] == 0


def test_needs_food_true_when_food_at_threshold():
    """
    Given an inventory with food exactly at FOOD_THRESHOLD
    When needs_food is called
    Then it returns True
    """
    inventory = {Resource.FOOD: FOOD_THRESHOLD}
    assert needs_food(inventory) is True


def test_needs_food_true_when_food_below_threshold():
    """
    Given an inventory with food strictly below FOOD_THRESHOLD
    When needs_food is called
    Then it returns True
    """
    inventory = {Resource.FOOD: FOOD_THRESHOLD - 1}
    assert needs_food(inventory) is True


def test_needs_food_false_when_food_above_threshold():
    """
    Given an inventory with food strictly above FOOD_THRESHOLD
    When needs_food is called
    Then it returns False
    """
    inventory = {Resource.FOOD: FOOD_THRESHOLD + 1}
    assert needs_food(inventory) is False


def test_needs_food_safe_true_when_food_at_safe_threshold():
    """
    Given an inventory with food exactly at FOOD_SAFE_THRESHOLD
    When needs_food_safe is called
    Then it returns True
    """
    inventory = {Resource.FOOD: FOOD_SAFE_THRESHOLD}
    assert needs_food_safe(inventory) is True


def test_needs_food_safe_false_when_food_above_safe_threshold():
    """
    Given an inventory with food above FOOD_SAFE_THRESHOLD
    When needs_food_safe is called
    Then it returns False
    """
    inventory = {Resource.FOOD: FOOD_SAFE_THRESHOLD + 1}
    assert needs_food_safe(inventory) is False


def test_needs_food_safe_true_when_food_below_safe_threshold():
    """
    Given an inventory with food below FOOD_SAFE_THRESHOLD but above FOOD_THRESHOLD
    When needs_food_safe is called
    Then it returns True
    """
    inventory = {Resource.FOOD: FOOD_THRESHOLD + 1}
    assert needs_food_safe(inventory) is True
