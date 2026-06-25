"""Coordination state: gather team members and trigger incantation."""
from ia.communication.broadcast import (
    MessageType,
    format_message,
    parse_broadcast,
)
from ia.config import (
    COORDINATION_MAX_WAIT_TICKS,
    COORDINATION_REBROADCAST_TICKS,
    COORDINATION_WAIT_LEAD_TICKS,
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
        ticks_waited = 0

        while joined < required and ticks_waited < COORDINATION_MAX_WAIT_TICKS:
            self._send_broadcast(MessageType.LEAD, str(self._bot.bot_id))
            ticks_waited += 7

            for _ in range(COORDINATION_REBROADCAST_TICKS):
                ticks_waited += 1
                if self._bot.food_critical():
                    return State.EATING

                while (line := self._bot.client.pop_notification()) \
                        is not None:
                    msg = parse_broadcast(line)
                    if not msg or msg.level != self._bot.level:
                        continue
                    if msg.msg_type == MessageType.JOIN and msg.direction == 0:
                        joined += 1
                        if joined >= required:
                            return self._incantate()
                    elif (
                        msg.msg_type == MessageType.LEAD
                        and msg.direction != 0
                    ):
                        rival_id = self._parse_rival_id(msg.data)
                        is_rival = (
                            rival_id is not None
                            and rival_id < self._bot.bot_id
                        )
                        if is_rival:
                            self._discard_pending_notifications()
                            return self._follow(msg.direction, rival_id)

        if joined >= required:
            return self._incantate()
        return State.EXPLORATION

    def _follow(self, initial_direction: int, leader_id: int) -> State:
        """Follow direction K from each LEAD broadcast until K=0, then JOIN."""
        direction = initial_direction
        ticks_waited = 0

        while ticks_waited < COORDINATION_MAX_WAIT_TICKS:
            if direction == 0:
                self._send_broadcast(MessageType.JOIN, "")
                return self._await_incantation()

            self._move_toward(direction)
            ticks_waited += len(broadcast_direction_to_moves(direction)) * 7

            self._discard_pending_notifications()

            direction, leader_id, state = self._await_lead_update(
                direction, leader_id
            )
            ticks_waited += COORDINATION_WAIT_LEAD_TICKS
            if state is not None:
                return state

        return State.EXPLORATION

    def _await_lead_update(
        self, direction: int, leader_id: int
    ) -> tuple[int, int, State | None]:
        """Poll for a fresher LEAD broadcast, return the chosen direction."""
        found_lead = False

        for _ in range(COORDINATION_WAIT_LEAD_TICKS):
            if self._bot.food_critical():
                return direction, leader_id, State.EATING

            while (line := self._bot.client.pop_notification()) is not None:
                msg = parse_broadcast(line)
                if not (
                    msg
                    and msg.msg_type == MessageType.LEAD
                    and msg.level == self._bot.level
                ):
                    continue
                rival_id = self._parse_rival_id(msg.data)
                if rival_id is None:
                    continue
                if rival_id <= leader_id:
                    leader_id = rival_id
                    direction = msg.direction
                    found_lead = True

            if found_lead:
                break

        return direction, leader_id, None

    @staticmethod
    def _parse_rival_id(data: str) -> int | None:
        """Parse the bot_id carried by a rival LEAD broadcast, if valid."""
        try:
            return int(data)
        except ValueError:
            return None

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
