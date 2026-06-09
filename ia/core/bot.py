"""Core bot logic for the Zappy AI client."""
from __future__ import annotations
from typing import TYPE_CHECKING
from ia.shared.enum import Direction
from ia.network.exceptions import PlayerDeadError

if TYPE_CHECKING:
    from ia.network.client import ZappyClient


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

    def _init_map(self) -> list[list[dict]]:
        """Creates an empty width×height grid. (ZAP-19 Bonus, stub)"""
        return [
            [{"resources": {}, "visited": False} for _ in range(self.width)]
            for _ in range(self.height)
        ]

    def _assign_role(self, client_num: int) -> str:
        """
        Role assignment based on the connection slot. (ZAP-21 Bonus, stub)
        Actual logic to be implemented in states/coordination.py.
        """
        # Placeholder — will be replaced by real specialization logic
        return "generic"

    def run(self) -> None:
        """Main reception loop; exits cleanly when the player dies."""
        try:
            while True:
                line = self._client.recv()
                if line is None:
                    break
                print(f"Received: {line}")
        except PlayerDeadError:
            pass
