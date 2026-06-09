from config import FOOD_THRESHOLD, FOOD_SAFE_THRESHOLD
from shared.enum import RESOURCES


def parse_inventory(response: str) -> dict[str, int]:
    """Parse the inventory response from the server."""
    inventory = {resource: 0 for resource in RESOURCES}
    if not response:
        return inventory

    items = response.strip().strip("[]").split(",")
    for item in items:
        parts = item.strip().split()
        if len(parts) == 2:
            resource, quantity = parts
            if resource in inventory:
                inventory[resource] = int(quantity)
    return inventory

def needs_food(inventory: dict[str, int]) -> bool:
    return inventory.get("food", 0) <= FOOD_THRESHOLD

def needs_food_safe(inventory: dict[str, int]) -> bool:
    return inventory.get("food", 0) <= FOOD_SAFE_THRESHOLD
