"""Coordination state: gather team members and trigger incantation."""
from ia.communication.broadcast import (
    MessageType,
    format_message,
    parse_broadcast,
)
from ia.config import (
    COORDINATION_MAX_WAIT_STEPS,
    COORDINATION_POLL_TIMEOUT,
    COORDINATION_REBROADCAST_STEPS,
)
from ia.core.bot import Bot
from ia.game.elevation import ELEVATION_REQUIREMENTS
from ia.game.navigation import broadcast_direction_to_moves
from ia.parsing.inventory import needs_food
from ia.shared.enum import State


class CoordinationState:  # pylint: disable=too-few-public-methods
    """Broadcast LEAD, count arrivals, incantate when quorum is reached."""

    def __init__(self, bot: Bot) -> None:
        self._bot = bot

    def handle(self) -> State:
        """Broadcast READY, lead the ritual or follow an existing leader."""
        if needs_food(self._bot.inventory):
            return State.EATING

        level = self._bot.level
        required = ELEVATION_REQUIREMENTS.get(level, {}).get(
            "players", 1
        )

        if required == 1:
            return self._incantate()

        self._send_broadcast(MessageType.READY, "")
        self._send_broadcast(MessageType.FORK_NEEDED, "")
        return self._lead(required)

    def _lead(self, required: int) -> State:
        """Broadcast LEAD, count JOINs, request a fork if quorum is missed."""
        joined = 1
        steps = 0

        while joined < required and steps < COORDINATION_MAX_WAIT_STEPS:
            if steps % COORDINATION_REBROADCAST_STEPS == 0:
                self._send_broadcast(
                    MessageType.LEAD, str(self._bot.client_num)
                )
                if self._bot.food_critical():
                    return State.EATING

            line = self._next_event()
            if line is None:
                if not self._bot.client.connected:
                    return State.EXPLORATION
                steps += 1
                continue

            msg = parse_broadcast(line)
            if not msg or msg.level != self._bot.level:
                steps += 1
                continue

            if msg.msg_type == MessageType.JOIN and msg.direction == 0:
                joined += 1
            elif msg.msg_type == MessageType.LEAD and msg.direction != 0:
                if self._loses_tie_break(msg.data):
                    self._discard_pending_notifications()
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

            if steps % COORDINATION_REBROADCAST_STEPS == 0 and (
                self._bot.food_critical()
            ):
                return State.EATING

            self._move_toward(direction)

            line = self._next_event()
            if line is None:
                if not self._bot.client.connected:
                    return State.EXPLORATION
                steps += 1
                continue

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
            self._bot.client.send(move.value)
            self._bot.client.recv_ack()

    def _send_broadcast(self, msg_type: MessageType, data: str) -> None:
        """Send a ZAPPY broadcast and consume the server acknowledgement."""
        payload = format_message(msg_type, self._bot.level, data)
        self._bot.client.send(f"Broadcast {payload}")
        self._bot.client.recv_ack()

    def _next_event(self) -> str | None:
        """Return a notification queued during an ack wait, or poll fresh."""
        queued = self._bot.client.pop_notification()
        if queued is not None:
            return queued
        return self._bot.client.recv_timeout(COORDINATION_POLL_TIMEOUT)

    def _loses_tie_break(self, rival_data: str) -> bool:
        """True when the rival leader's client_num outranks ours."""
        try:
            rival_client_num = int(rival_data)
        except ValueError:
            return True
        return rival_client_num < self._bot.client_num

    def _discard_pending_notifications(self) -> None:
        """Drop notifications captured while still a leader candidate."""
        while self._bot.client.pop_notification() is not None:
            pass

    def _incantate(self) -> State:
        """Become incantation chef; the FSM runs the ritual next tick."""
        self._bot.is_incantation_chef = True
        return State.INCANTATION

    def _await_incantation(self) -> State:
        """Become incantation follower; the FSM awaits the ritual next tick."""
        self._bot.is_incantation_chef = False
        return State.INCANTATION
