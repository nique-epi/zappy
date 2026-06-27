"""Fixed role specialisation for team bots (ZAP-21 bonus)."""
from __future__ import annotations

from typing import TYPE_CHECKING

from ia.config import FARMER_TEAM_SHARE
from ia.parsing.look import LookTile
from ia.shared.enum import Resource, Role, State

if TYPE_CHECKING:
    from ia.states.exploration import ExplorationState


def assign_role(client_num: int) -> Role:
    """Pick a fixed role from the free-slot count reported at connection."""
    if client_num <= 0:
        return Role.CHEF
    if client_num % FARMER_TEAM_SHARE == 0:
        return Role.FARMER
    return Role.COLLECTOR


def handle_collector(explorer: ExplorationState) -> State:
    """Collectors run the standard stone-seeking exploration."""
    return explorer.explore_for_stones(allow_fork=True)


def handle_chef(explorer: ExplorationState) -> State:
    """The chef gathers stones but never forks, staying ready to lead."""
    return explorer.explore_for_stones(allow_fork=False)


def handle_farmer(explorer: ExplorationState) -> State:
    """Farmers forage food only and never pick up stones."""
    explorer.maybe_broadcast_farmer_heartbeat()
    tiles = explorer.look()
    if tiles is None:
        return State.EXPLORATION
    food_index = _first_food_tile(tiles)
    if food_index is None:
        explorer.wander()
        return State.EXPLORATION
    explorer.go_to_tile(food_index)
    explorer.take(Resource.FOOD.value)
    return State.EXPLORATION


def _first_food_tile(tiles: list[LookTile]) -> int | None:
    """Return the index of the first visible tile holding food, if any."""
    return next(
        (
            index for index, tile in enumerate(tiles)
            if Resource.FOOD.value in tile["objects"]
        ),
        None,
    )
