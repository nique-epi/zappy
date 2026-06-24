from ia.config import FOOD_THRESHOLD, FOOD_SAFE_THRESHOLD
from ia.shared.enum import Resource


def parse_inventory(response: str) -> dict[Resource, int]:
    """Parse the inventory response from the server."""
    inventory = dict.fromkeys(Resource, 0)
    if not response:
        return inventory

    items = response.strip().strip("[]").split(",")
    for item in items:
        parts = item.strip().split()
        if len(parts) == 2:
            name, quantity = parts
            try:
                inventory[Resource(name)] = int(quantity)
            except ValueError:
                pass
    return inventory


def needs_food(inventory: dict[Resource, int]) -> bool:
    return inventory.get(Resource.FOOD, 0) <= FOOD_THRESHOLD


def needs_food_safe(inventory: dict[Resource, int]) -> bool:
    return inventory.get(Resource.FOOD, 0) < FOOD_SAFE_THRESHOLD
