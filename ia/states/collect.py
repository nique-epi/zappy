from ia.game.elevation import stones_missing
from ia.game.navigation import tile_to_moves
from ia.parsing.look import parse_look
from ia.shared.enum import State
from ia.core.bot import Bot


class CollectState:  # pylint: disable=too-few-public-methods
    def __init__(self, bot: Bot):
        self.bot = bot

    def handle(self) -> State:
        """Move to the target tile, take all useful stones."""
        if not self._move_to_target():
            return State.EXPLORATION

        self.bot.client.send("Look")
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

    def _move_to_target(self) -> bool:
        """Execute the move sequence; return False if any move fails."""
        for move in tile_to_moves(self.bot.collect_target):
            self.bot.client.send(move.value)
            response = self.bot.client.recv()
            if response is None or response.strip() == "ko":
                return False
        return True

    def _take_stones_on_current_tile(self, objects: list[str]) -> None:
        """Take every useful stone present on the current tile."""
        missing = stones_missing(
            self.bot.level,
            {r.value: v for r, v in self.bot.inventory.items()},
        )
        for obj in objects:
            if obj not in missing:
                continue
            self.bot.client.send(f"Take {obj}")
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
