from ia.game.elevation import stones_missing
from ia.game.navigation import tile_to_moves
from ia.parsing.look import parse_look
from ia.shared.enum import State
from ia.core.bot import Bot


class CollectState:  # pylint: disable=too-few-public-methods
    def __init__(self, bot: Bot, tile_index: int):
        self.bot = bot
        self._tile_index = tile_index

    def handle(self) -> State:
        """Move to the target tile, take all useful stones."""
        self._move_to_target()

        self.bot.client.send("Look\n")
        response = self.bot.client.recv()
        if response is None:
            return State.EXPLORATION

        tiles = parse_look(
            response, self.bot.pos, self.bot.orientation, self.bot.level
        )
        if tiles:
            self._take_stones_on_current_tile(tiles[0]["objects"])

        missing = stones_missing(
            self.bot.level,
            {r.value: v for r, v in self.bot.inventory.items()},
        )
        if not missing:
            return State.COORDINATION
        return State.EXPLORATION

    def _move_to_target(self) -> None:
        """Execute the move sequence to reach the target tile."""
        for move in tile_to_moves(self._tile_index):
            self.bot.client.send(move.value + "\n")
            self.bot.client.recv()

    def _take_stones_on_current_tile(self, objects: list[str]) -> None:
        """Take every useful stone present on the current tile."""
        missing = stones_missing(
            self.bot.level,
            {r.value: v for r, v in self.bot.inventory.items()},
        )
        for obj in objects:
            if obj not in missing:
                continue
            self.bot.client.send(f"Take {obj}\n")
            response = self.bot.client.recv()
            if response and response.strip() == "ok":
                resource = next(
                    (r for r in self.bot.inventory if r.value == obj), None
                )
                if resource is not None:
                    self.bot.inventory[resource] += 1
                    missing = stones_missing(
                        self.bot.level,
                        {r.value: v for r, v in self.bot.inventory.items()},
                    )
