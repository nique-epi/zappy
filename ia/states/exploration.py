from ia.config import INVENTORY_CHECK_INTERVAL, EXPLORATION_TURN_INTERVAL
from ia.game.elevation import stones_missing
from ia.parsing.look import parse_look
from ia.shared.enum import State
from ia.core.bot import Bot


class ExplorationState:  # pylint: disable=too-few-public-methods
    def __init__(self, bot: Bot):
        self.bot = bot
        self._steps = 0
        self._turn_counter = 0

    def handle(self) -> State:
        """Send Look, transition to COLLECTING if a useful stone is visible."""
        self._steps += 1
        if self._steps >= INVENTORY_CHECK_INTERVAL:
            self._steps = 0
            return State.SURVIVAL

        self.bot.client.send("Look\n")
        response = self.bot.client.recv()
        if response is None:
            return State.EXPLORATION

        tiles = parse_look(
            response, self.bot.pos, self.bot.orientation, self.bot.level
        )
        missing = stones_missing(
            self.bot.level,
            {r.value: v for r, v in self.bot.inventory.items()},
        )
        useful_index = next(
            (
                i for i, tile in enumerate(tiles)
                if any(stone in tile["objects"] for stone in missing)
            ),
            None,
        )
        if useful_index is not None:
            return State.COLLECTING

        self._explore()
        return State.EXPLORATION

    def next_exploration_target(self):
        """Return the next exploration target; stub for ZAP-19."""
        # ZAP-19 : retournera nearest_resource() si carte mentale disponible
        return None

    def _explore(self) -> None:
        """Advance and periodically turn to avoid looping in place."""
        self._turn_counter += 1
        if self._turn_counter % EXPLORATION_TURN_INTERVAL == 0:
            side = (self._turn_counter // EXPLORATION_TURN_INTERVAL) % 2
            turn = "Left\n" if side == 0 else "Right\n"
            self.bot.client.send(turn)
            self.bot.client.recv()
        self.bot.client.send("Forward\n")
        self.bot.client.recv()

    def _handle_farmer(self) -> State:
        """Stub for ZAP-21 farmer role."""
        return State.EXPLORATION

    def _handle_collector(self) -> State:
        """Stub for ZAP-21 collector role."""
        return State.EXPLORATION
