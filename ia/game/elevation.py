MAX_LEVEL = 8

ELEVATION_REQUIREMENTS = {
    1: {
        "players": 1, "linemate": 1, "deraumere": 0,
        "sibur": 0, "mendiane": 0, "phiras": 0, "thystame": 0,
    },
    2: {
        "players": 2, "linemate": 1, "deraumere": 1,
        "sibur": 1, "mendiane": 0, "phiras": 0, "thystame": 0,
    },
    3: {
        "players": 2, "linemate": 2, "deraumere": 0,
        "sibur": 1, "mendiane": 0, "phiras": 2, "thystame": 0,
    },
    4: {
        "players": 4, "linemate": 1, "deraumere": 1,
        "sibur": 2, "mendiane": 0, "phiras": 1, "thystame": 0,
    },
    5: {
        "players": 4, "linemate": 1, "deraumere": 2,
        "sibur": 1, "mendiane": 3, "phiras": 0, "thystame": 0,
    },
    6: {
        "players": 6, "linemate": 1, "deraumere": 2,
        "sibur": 3, "mendiane": 0, "phiras": 1, "thystame": 0,
    },
    7: {
        "players": 6, "linemate": 2, "deraumere": 2,
        "sibur": 2, "mendiane": 2, "phiras": 2, "thystame": 1,
    },
}


def stones_missing(level, inventory):
    """Given a level and an inventory of stones"""
    if level >= MAX_LEVEL:
        return {}
    requirements = ELEVATION_REQUIREMENTS.get(level)
    if not requirements:
        raise ValueError(f"Invalid level: {level}")

    missing_stones = {}
    for stone, required_amount in requirements.items():
        if stone == "players":
            continue
        current_amount = inventory.get(stone, 0)
        if current_amount < required_amount:
            missing_stones[stone] = required_amount - current_amount

    return missing_stones


def is_ready_to_elevate(level, inventory):
    """Given a level and an inventory of stones"""
    if level >= MAX_LEVEL:
        return False
    requirements = ELEVATION_REQUIREMENTS.get(level)
    if not requirements:
        raise ValueError(f"Invalid level: {level}")
    for stone, required_amount in requirements.items():
        if stone == "players":
            continue
        current_amount = inventory.get(stone, 0)
        if current_amount < required_amount:
            return False
    return True
