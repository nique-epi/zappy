from ia.config import FOOD_THRESHOLD, FOOD_SAFE_THRESHOLD
from ia.shared.enum import RESOURCES


def parse_inventory(response: str) -> dict[str, int]:
    """Parse the inventory response from the server."""
    inventory = dict.fromkeys(RESOURCES, 0)
    if not response:
        return inventory

    items = response.strip().strip("[]").split(",")
    for item in items:
        parts = item.strip().split()
        if len(parts) == 2:
            resource, quantity = parts
            if resource in inventory:
                try:
                    inventory[resource] = int(quantity)
                except ValueError:
                    pass
    return inventory


def needs_food(inventory: dict[str, int]) -> bool:
    return inventory.get("food", 0) <= FOOD_THRESHOLD


def needs_food_safe(inventory: dict[str, int]) -> bool:
    return inventory.get("food", 0) <= FOOD_SAFE_THRESHOLD
