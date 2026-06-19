"""Coordination state: gather team members and trigger incantation."""
from ia.communication.broadcast import (
    MessageType,
    format_message,
    parse_broadcast,
)
from ia.config import (
    COORDINATION_MAX_WAIT_STEPS,
    COORDINATION_REBROADCAST_STEPS,
)
from ia.core.bot import Bot
from ia.game.elevation import ELEVATION_REQUIREMENTS
from ia.game.navigation import broadcast_direction_to_moves
from ia.shared.enum import State


class CoordinationState:  # pylint: disable=too-few-public-methods
    """Broadcast LEAD, count arrivals, incantate when quorum is reached."""

    def __init__(self, bot: Bot) -> None:
        self._bot = bot

    def handle(self) -> State:
        """Broadcast READY, lead the ritual or follow an existing leader."""
        level = self._bot.level
        required = ELEVATION_REQUIREMENTS.get(level, {}).get(
            "players", 1
        )

        if required == 1:
            return self._incantate()

        self._send_broadcast(MessageType.READY, "")
        return self._lead(required)

    def _lead(self, required: int) -> State:
        """Broadcast LEAD periodically and count zero-direction JOINs."""
        joined = 1
        steps = 0

        while joined < required and steps < COORDINATION_MAX_WAIT_STEPS:
            if steps % COORDINATION_REBROADCAST_STEPS == 0:
                self._send_broadcast(MessageType.LEAD, "")

            line = self._bot.client.recv()
            if line is None:
                return State.EXPLORATION

            msg = parse_broadcast(line)
            if not msg or msg.level != self._bot.level:
                steps += 1
                continue

            if msg.msg_type == MessageType.JOIN and msg.direction == 0:
                joined += 1
            elif msg.msg_type == MessageType.LEAD:
                return self._follow(msg.direction)

            steps += 1

        if joined >= required:
            return self._incantate()
        return State.EXPLORATION

    def _follow(self, initial_direction: int) -> State:
        """Follow direction K from each LEAD broadcast until K=0, then JOIN."""
        direction = initial_direction
        steps = 0

        while steps < COORDINATION_MAX_WAIT_STEPS:
            if direction == 0:
                self._send_broadcast(MessageType.JOIN, "")
                return self._await_incantation()

            self._move_toward(direction)

            line = self._bot.client.recv()
            if line is None:
                return State.EXPLORATION

            msg = parse_broadcast(line)
            if (
                msg
                and msg.msg_type == MessageType.LEAD
                and msg.level == self._bot.level
            ):
                direction = msg.direction

            steps += 1

        return State.EXPLORATION

    def _move_toward(self, direction: int) -> None:
        """Send movement commands toward broadcast direction K."""
        for move in broadcast_direction_to_moves(direction):
            self._bot.client.send(f"{move.value}\n")
            self._bot.client.recv()

    def _send_broadcast(self, msg_type: MessageType, data: str) -> None:
        """Send a ZAPPY broadcast and consume the server acknowledgement."""
        payload = format_message(msg_type, self._bot.level, data)
        self._bot.client.send(f"Broadcast {payload}\n")
        self._bot.client.recv()

    def _incantate(self) -> State:
        """Become incantation chef; the FSM runs the ritual next tick."""
        self._bot.is_incantation_chef = True
        return State.INCANTATION

    def _await_incantation(self) -> State:
        """Become incantation follower; the FSM awaits the ritual next tick."""
        self._bot.is_incantation_chef = False
        return State.INCANTATION
