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
        from ia.parsing.inventory import parse_inventory
        joined = 1
        ticks_waited = 0
        MAX_WAIT_TICKS = 1000
        REBROADCAST_TICKS = 40

        while joined < required and ticks_waited < MAX_WAIT_TICKS:
            self._send_broadcast(MessageType.LEAD, str(self._bot.bot_id))
            ticks_waited += 7

            for _ in range(REBROADCAST_TICKS):
                self._bot.client.send("Inventory")
                resp = self._bot.client.recv_ack()
                ticks_waited += 1
                if resp:
                    self._bot.inventory = parse_inventory(resp)
                
                if self._bot.food_critical():
                    return State.EATING

                while (line := self._bot.client.pop_notification()) is not None:
                    msg = parse_broadcast(line)
                    if not msg or msg.level != self._bot.level:
                        continue
                    if msg.msg_type == MessageType.JOIN and msg.direction == 0:
                        joined += 1
                        if joined >= required:
                            return self._incantate()
                    elif msg.msg_type == MessageType.LEAD and msg.direction != 0:
                        try:
                            rival_id = int(msg.data)
                        except ValueError:
                            continue
                        if rival_id < self._bot.bot_id:
                            self._discard_pending_notifications()
                            return self._follow(msg.direction, rival_id)

        if joined >= required:
            return self._incantate()
        return State.EXPLORATION

    def _follow(self, initial_direction: int, leader_id: int) -> State:
        """Follow direction K from each LEAD broadcast until K=0, then JOIN."""
        from ia.parsing.inventory import parse_inventory
        direction = initial_direction
        ticks_waited = 0
        MAX_WAIT_TICKS = 1000
        WAIT_LEAD_TICKS = 50

        while ticks_waited < MAX_WAIT_TICKS:
            if direction == 0:
                self._send_broadcast(MessageType.JOIN, "")
                return self._await_incantation()

            self._move_toward(direction)
            moves = broadcast_direction_to_moves(direction)
            ticks_waited += len(moves) * 7
            
            self._discard_pending_notifications()
            
            got_lead = False
            for _ in range(WAIT_LEAD_TICKS):
                self._bot.client.send("Inventory")
                resp = self._bot.client.recv_ack()
                ticks_waited += 1
                if resp:
                    self._bot.inventory = parse_inventory(resp)
                
                if self._bot.food_critical():
                    return State.EATING
                    
                while (line := self._bot.client.pop_notification()) is not None:
                    msg = parse_broadcast(line)
                    if (msg and msg.msg_type == MessageType.LEAD and msg.level == self._bot.level):
                        try:
                            rival_id = int(msg.data)
                        except ValueError:
                            continue
                        if rival_id < leader_id:
                            leader_id = rival_id
                            direction = msg.direction
                            got_lead = True
                        elif rival_id == leader_id:
                            direction = msg.direction
                            got_lead = True
                
                if got_lead:
                    break

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
