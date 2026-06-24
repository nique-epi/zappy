from ia.game.elevation import ELEVATION_REQUIREMENTS, stones_missing
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

        self._eject_competitors()

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
        if missing:
            return State.EXPLORATION

        self._drop_required_stones()
        return State.COORDINATION

    def _move_to_target(self) -> bool:
        """Execute the move sequence; return False if any move fails."""
        for move in tile_to_moves(self.bot.collect_target):
            self.bot.client.send(move.value)
            response = self.bot.client.recv()
            if response is None or response.strip() == "ko":
                return False
        return True

    def _eject_competitors(self) -> None:
        """Clear rival players from the tile before claiming its stones."""
        self.bot.client.send("Eject")
        self.bot.client.recv()

    def _drop_required_stones(self) -> None:
        """Set every stone needed for the next elevation onto the ground.

        The incantation's prerequisites are checked on the tile, not in a
        player's inventory, so the gathered stones must be laid down here.
        """
        requirements = ELEVATION_REQUIREMENTS[self.bot.level]
        for stone, amount in requirements.items():
            if stone == "players" or amount == 0:
                continue
            resource = next(
                (r for r in self.bot.inventory if r.value == stone), None
            )
            if resource is None:
                continue
            for _ in range(amount):
                self.bot.client.send(f"Set {stone}")
                response = self.bot.client.recv()
                if response and response.strip() == "ok":
                    self.bot.inventory[resource] -= 1

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
