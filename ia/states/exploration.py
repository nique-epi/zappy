from ia.bonus import roles
from ia.communication.broadcast import (
    BroadcastMessage,
    MessageType,
    format_message,
    parse_broadcast,
)
from ia.config import (
    EXPLORATION_TURN_INTERVAL,
    FORK_CHECK_INTERVAL,
    FORK_LIMIT,
    FORK_MAX_LEVEL,
    INVENTORY_CHECK_INTERVAL,
    MAX_FORK_DEPTH,
    ROLE_HEARTBEAT_INTERVAL,
    ROLE_PROMOTION_STAGGER,
    ROLE_VACANCY_TIMEOUT,
)
from ia.game.elevation import stones_missing
from ia.game.navigation import broadcast_direction_to_moves, tile_to_moves
from ia.parsing.connect import parse_connect_nbr
from ia.parsing.eject import parse_eject
from ia.parsing.look import LookTile, parse_look
from ia.shared.enum import Move, Role, State
from ia.core.bot import Bot

_ROLE_HANDLERS = {
    Role.FARMER: roles.handle_farmer,
    Role.COLLECTOR: roles.handle_collector,
    Role.CHEF: roles.handle_chef,
}


class ExplorationState:
    def __init__(self, bot: Bot):
        self.bot = bot
        self._steps = 0
        self._turn_counter = 0
        self._fork_counter = 0
        self._farmer_heartbeat_counter = 0
        self.farmer_vacancy_ticks = 0

    def handle(self) -> State:
        """Dispatch the exploration tick to the bot's role handler."""
        self._steps += 1
        if self._steps >= INVENTORY_CHECK_INTERVAL:
            self._steps = 0
            return State.SURVIVAL
        handler = _ROLE_HANDLERS.get(self.bot.role, roles.handle_collector)
        result = handler(self)
        self.track_farmer_vacancy()
        return result

    def explore_for_stones(self, allow_fork: bool = True) -> State:
        """Look for a missing stone; collect it or keep wandering."""
        if allow_fork:
            self._maybe_self_fork()
        tiles = self.look()
        if tiles is None:
            return State.EXPLORATION
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
        self.wander()
        return State.EXPLORATION

    def look(self) -> list[LookTile] | None:
        """Send Look, drain notifications, return parsed tiles or None."""
        self.bot.client.send("Look")
        response = self.bot.client.recv_ack()
        if response is None:
            return None
        self._drain_notifications()
        tiles = parse_look(
            response, self.bot.pos, self.bot.orientation, self.bot.level
        )
        if self.bot.mental_map_enabled:
            self.bot.world_map.update_from_look(tiles, self.bot.turn)
        return tiles

    def wander(self) -> None:
        """Run one exploration move step toward a target or while turning."""
        self._explore()

    def step(self, move: Move) -> None:
        """Send a single move command, tracking position and orientation."""
        if move == Move.FORWARD:
            self._send_forward()
        elif move == Move.LEFT:
            self._send_left()
        else:
            self._send_right()

    def go_to_tile(self, index: int) -> None:
        """Walk the move sequence that reaches a Look tile by its index."""
        for move in tile_to_moves(index):
            self.step(move)

    def take(self, item: str) -> None:
        """Take one object of the given kind from the current tile."""
        self.bot.client.send(f"Take {item}")
        self.bot.client.recv_ack()

    def broadcast(self, msg_type: MessageType, data: str = "") -> None:
        """Send a ZAPPY broadcast and consume the acknowledgement."""
        payload = format_message(msg_type, self.bot.level, data)
        self.bot.client.send(f"Broadcast {payload}")
        self.bot.client.recv_ack()

    def maybe_broadcast_farmer_heartbeat(self) -> None:
        """Periodically re-announce the farmer role to keep peers aware."""
        self._farmer_heartbeat_counter += 1
        if self._farmer_heartbeat_counter % ROLE_HEARTBEAT_INTERVAL != 0:
            return
        self.broadcast(MessageType.ROLE_CLAIM, Role.FARMER.value)

    def track_farmer_vacancy(self) -> None:
        """Promote this collector to farmer if none has been heard recently."""
        if self.bot.role != Role.COLLECTOR:
            return
        self.farmer_vacancy_ticks += 1
        threshold = (
            ROLE_VACANCY_TIMEOUT + self.bot.bot_id % ROLE_PROMOTION_STAGGER
        )
        if self.farmer_vacancy_ticks < threshold:
            return
        self.farmer_vacancy_ticks = 0
        self.bot.role = Role.FARMER
        self.broadcast(MessageType.ROLE_CLAIM, Role.FARMER.value)

    def _can_fork(self) -> bool:
        """True when this bot is a low-level, shallow-enough candidate."""
        return (
            self.bot.level <= FORK_MAX_LEVEL
            and self.bot.fork_count < FORK_LIMIT
            and self.bot.fork_depth < MAX_FORK_DEPTH
        )

    def _maybe_self_fork(self) -> None:
        """Periodically claim a free slot, or fork one if none remains."""
        self._fork_counter += 1
        if self._fork_counter % FORK_CHECK_INTERVAL != 0:
            return
        if not self._can_fork():
            return
        self.bot.client.send("Connect_nbr")
        response = self.bot.client.recv_ack()
        if response is None:
            return
        try:
            free_slots = parse_connect_nbr(response)
        except ValueError:
            return
        if free_slots == 0:
            self._fork()
        elif self.bot.spawn_player is not None:
            self.bot.spawn_player(self.bot.fork_depth + 1)

    def _drain_notifications(self) -> None:
        """Process broadcasts and ejects queued since the last tick."""
        while (line := self.bot.client.pop_notification()) is not None:
            broadcast = parse_broadcast(line)
            if broadcast is not None:
                self._handle_broadcast(broadcast)
                continue
            if line.startswith("eject:"):
                self._handle_eject_notification(line)

    def _handle_broadcast(self, message: BroadcastMessage) -> None:
        """React to a teammate broadcast: role heartbeat or fork request."""
        if (
            message.msg_type == MessageType.ROLE_CLAIM
            and message.data == Role.FARMER.value
        ):
            self.farmer_vacancy_ticks = 0
            return
        if message.msg_type != MessageType.FORK_NEEDED or not self._can_fork():
            return
        self.bot.client.send("Connect_nbr")
        response = self.bot.client.recv_ack()
        if response is None:
            return
        try:
            free_slots = parse_connect_nbr(response)
        except ValueError:
            return
        if free_slots == 0:
            self._fork()

    def _handle_eject_notification(self, line: str) -> None:
        """Apply the server-driven displacement carried by an eject line."""
        direction = parse_eject(line)
        if direction is None:
            return
        for move in broadcast_direction_to_moves(direction):
            if move == Move.FORWARD:
                self.bot.advance()
            elif move == Move.LEFT:
                self.bot.turn_left()
            else:
                self.bot.turn_right()

    def _fork(self) -> None:
        """Lay an egg so a new teammate can connect, then keep exploring."""
        self.bot.client.send("Fork")
        self.bot.client.recv_ack()
        self.bot.fork_count += 1
        if self.bot.spawn_player is not None:
            self.bot.spawn_player(self.bot.fork_depth + 1)

    def _explore(self) -> None:
        """Head for a known resource, or wander while turning periodically."""
        target = (
            self.bot.next_exploration_target()
            if self.bot.mental_map_enabled
            else None
        )
        if target is not None and target != self.bot.pos:
            self._move_towards(target)
            return
        self._turn_counter += 1
        if self._turn_counter % EXPLORATION_TURN_INTERVAL == 0:
            side = (self._turn_counter // EXPLORATION_TURN_INTERVAL) % 2
            if side == 0:
                self._send_left()
            else:
                self._send_right()
        self._send_forward()

    def _move_towards(self, target: tuple[int, int]) -> None:
        """Send the next move command on the path to target."""
        moves = self.bot.world_map.path_to(
            self.bot.pos, self.bot.orientation, target
        )
        if not moves:
            self._send_forward()
            return
        self.step(moves[0])

    def _send_forward(self) -> None:
        self.bot.client.send("Forward")
        if self.bot.client.recv_ack() is not None:
            self.bot.advance()

    def _send_left(self) -> None:
        self.bot.client.send("Left")
        if self.bot.client.recv_ack() is not None:
            self.bot.turn_left()

    def _send_right(self) -> None:
        self.bot.client.send("Right")
        if self.bot.client.recv_ack() is not None:
            self.bot.turn_right()
