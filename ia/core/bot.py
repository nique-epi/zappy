"""Core bot logic for the Zappy AI client."""
from __future__ import annotations

from typing import TYPE_CHECKING, Callable

from ia.bonus.map import WorldMap
from ia.config import STALE_TURNS
from ia.game.elevation import stones_missing
from ia.game.navigation import ORIENTATION_DELTAS
from ia.parsing.inventory import needs_food, parse_inventory
from ia.shared.enum import Direction, Resource, State

if TYPE_CHECKING:
    from ia.network.client import ZappyClient

_DIRECTION_ORDER = [
    Direction.NORTH, Direction.EAST, Direction.SOUTH, Direction.WEST
]


# pylint: disable=too-many-instance-attributes,too-few-public-methods
# pylint: disable=too-many-arguments,too-many-positional-arguments
class Bot:
    """Class representing the AI bot."""

    def __init__(
        self,
        width: int,
        height: int,
        client_num: int,
        client: "ZappyClient",
        mental_map: bool = False,
    ) -> None:
        self._client = client
        self.width = width
        self.height = height
        self.client_num = client_num
        self.mental_map_enabled = mental_map

        self.level = 1
        self.pos = (0, 0)
        self.orientation = Direction.NORTH
        self.turn = 0

        self.world_map = WorldMap(width, height)
        self.role: str = self._assign_role(client_num)
        self.inventory: dict[Resource, int] = dict.fromkeys(Resource, 0)
        self.state: State = State.SURVIVAL
        self.collect_target: int = 0
        self.is_incantation_chef: bool = True
        self.fork_count: int = 0
        self.fork_depth: int = 0
        self.spawn_player: Callable[[int], None] | None = None

    @property
    def client(self) -> "ZappyClient":
        """Expose the network client to state handlers."""
        return self._client

    def advance(self) -> None:
        """Move one tile forward in the current orientation, with wrap."""
        forward_dx, forward_dy, _, _ = ORIENTATION_DELTAS[self.orientation]
        x, y = self.pos
        self.pos = (
            (x + forward_dx) % self.width,
            (y + forward_dy) % self.height,
        )

    def turn_left(self) -> None:
        """Rotate the bot's orientation one step counter-clockwise."""
        index = _DIRECTION_ORDER.index(self.orientation)
        self.orientation = _DIRECTION_ORDER[
            (index - 1) % len(_DIRECTION_ORDER)
        ]

    def turn_right(self) -> None:
        """Rotate the bot's orientation one step clockwise."""
        index = _DIRECTION_ORDER.index(self.orientation)
        self.orientation = _DIRECTION_ORDER[
            (index + 1) % len(_DIRECTION_ORDER)
        ]

    def nearest_resource(self, resource: Resource) -> tuple[int, int] | None:
        """Find the nearest known, non-stale tile holding the resource."""
        return self.world_map.nearest_resource(
            self.pos, resource.value, self.turn, STALE_TURNS
        )

    def next_exploration_target(self) -> tuple[int, int] | None:
        """Pick the closest known tile holding a stone still missing."""
        missing = stones_missing(
            self.level, {r.value: v for r, v in self.inventory.items()}
        )
        best_target: tuple[int, int] | None = None
        best_distance = float("inf")
        for resource in Resource:
            if resource.value not in missing:
                continue
            target = self.nearest_resource(resource)
            if target is None:
                continue
            distance = self.world_map.toroidal_distance(self.pos, target)
            if distance < best_distance:
                best_distance = distance
                best_target = target
        return best_target

    def food_critical(self) -> bool:
        """Refreshes inventory via the server and reports if food is low."""
        self.client.send("Inventory")
        response = self.client.recv_ack()
        if response is not None:
            self.inventory = parse_inventory(response)
        return needs_food(self.inventory)

    def _assign_role(self, client_num: int) -> str:
        """Role assignment based on the connection slot. (ZAP-21 Bonus, stub)
        """
        _ = client_num
        return "generic"
