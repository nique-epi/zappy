"""Core bot logic for the Zappy AI client."""
from __future__ import annotations

from typing import TYPE_CHECKING

from ia.shared.enum import Direction, Resource, State

if TYPE_CHECKING:
    from ia.network.client import ZappyClient


# pylint: disable=too-many-instance-attributes,too-few-public-methods
class Bot:
    """Class representing the AI bot."""

    def __init__(
        self,
        width: int,
        height: int,
        client_num: int,
        client: "ZappyClient",
    ) -> None:
        self._client = client
        self.width = width
        self.height = height
        self.client_num = client_num

        self.level = 1
        self.pos = (0, 0)
        self.orientation = Direction.NORTH

        self.map: list[list[dict]] = self._init_map()
        self.role: str = self._assign_role(client_num)
        self.inventory: dict[Resource, int] = dict.fromkeys(Resource, 0)
        self.state: State = State.SURVIVAL
        self.collect_target: int = 0
        self.is_incantation_chef: bool = True
        self.fork_count: int = 0

    @property
    def client(self) -> "ZappyClient":
        """Expose the network client to state handlers."""
        return self._client

    def _init_map(self) -> list[list[dict]]:
        """Creates an empty width×height grid. (ZAP-19 Bonus, stub)"""
        return [
            [{"resources": {}, "visited": False} for _ in range(self.width)]
            for _ in range(self.height)
        ]

    def _assign_role(self, client_num: int) -> str:
        """Role assignment based on the connection slot. (ZAP-21 Bonus, stub)
        """
        _ = client_num
        return "generic"
