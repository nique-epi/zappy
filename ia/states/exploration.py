from ia.communication.broadcast import (
    BroadcastMessage,
    MessageType,
    parse_broadcast,
)
from ia.config import (
    EXPLORATION_TURN_INTERVAL,
    FORK_CHECK_INTERVAL,
    FORK_LIMIT,
    FORK_MAX_LEVEL,
    INVENTORY_CHECK_INTERVAL,
)
from ia.game.elevation import stones_missing
from ia.parsing.connect import parse_connect_nbr
from ia.parsing.look import parse_look
from ia.shared.enum import State
from ia.core.bot import Bot


class ExplorationState:  # pylint: disable=too-few-public-methods
    def __init__(self, bot: Bot):
        self.bot = bot
        self._steps = 0
        self._turn_counter = 0
        self._fork_counter = 0

    def handle(self) -> State:
        """Send Look, transition to COLLECTING if a useful stone is visible."""
        self._steps += 1
        if self._steps >= INVENTORY_CHECK_INTERVAL:
            self._steps = 0
            return State.SURVIVAL

        self._maybe_self_fork()

        self.bot.client.send("Look")
        response = self.bot.client.recv()
        if response is None:
            return State.EXPLORATION

        broadcast = parse_broadcast(response)
        if broadcast is not None:
            self._handle_broadcast(broadcast)
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
            self.bot.collect_target = useful_index
            return State.COLLECTING

        self._explore()
        return State.EXPLORATION

    def next_exploration_target(self):
        """Return the next exploration target; stub for ZAP-19."""
        # ZAP-19 : retournera nearest_resource() si carte mentale disponible
        return None

    def _can_fork(self) -> bool:
        """True when this bot is a low-level candidate below the fork cap."""
        return (
            self.bot.level <= FORK_MAX_LEVEL
            and self.bot.fork_count < FORK_LIMIT
        )

    def _maybe_self_fork(self) -> None:
        """Periodically fork when no free team slot remains."""
        self._fork_counter += 1
        if self._fork_counter % FORK_CHECK_INTERVAL != 0:
            return
        if not self._can_fork():
            return
        self.bot.client.send("Connect_nbr")
        response = self.bot.client.recv()
        if response is None:
            return
        try:
            free_slots = parse_connect_nbr(response)
        except ValueError:
            return
        if free_slots == 0:
            self._fork()

    def _handle_broadcast(self, message: BroadcastMessage) -> None:
        """Fork on a FORK_NEEDED call when this bot is available."""
        if message.msg_type == MessageType.FORK_NEEDED and self._can_fork():
            self._fork()

    def _fork(self) -> None:
        """Lay an egg so a new teammate can connect, then keep exploring."""
        self.bot.client.send("Fork")
        self.bot.client.recv()
        self.bot.fork_count += 1

    def _explore(self) -> None:
        """Advance and periodically turn to avoid looping in place."""
        self._turn_counter += 1
        if self._turn_counter % EXPLORATION_TURN_INTERVAL == 0:
            side = (self._turn_counter // EXPLORATION_TURN_INTERVAL) % 2
            turn = "Left" if side == 0 else "Right"
            self.bot.client.send(turn)
            self.bot.client.recv()
        self.bot.client.send("Forward")
        self.bot.client.recv()

    def _handle_farmer(self) -> State:
        """Stub for ZAP-21 farmer role."""
        return State.EXPLORATION

    def _handle_collector(self) -> State:
        """Stub for ZAP-21 collector role."""
        return State.EXPLORATION
