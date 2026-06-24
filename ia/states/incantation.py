"""Incantation state: send Incantation, wait for result, update bot level."""
import sys

from ia.config import COORDINATION_MAX_WAIT_STEPS, COORDINATION_POLL_TIMEOUT
from ia.core.bot import Bot
from ia.game.elevation import MAX_LEVEL
from ia.parsing.inventory import needs_food, needs_food_safe
from ia.shared.enum import State


class IncantationState:  # pylint: disable=too-few-public-methods
    """Send Incantation as chef or await its result as follower."""

    def __init__(self, bot: Bot) -> None:
        self._bot = bot

    def handle(self) -> State:
        """Send or await Incantation, gated by the safe food threshold."""
        if needs_food_safe(self._bot.inventory):
            return State.EATING

        if self._bot.is_incantation_chef:
            self._bot.client.send("Incantation")
        return self._wait_for_result()

    def _wait_for_result(self) -> State:
        """Read server lines until incantation concludes or timeout."""
        steps = 0
        while steps < COORDINATION_MAX_WAIT_STEPS:
            line = self._bot.client.pop_notification()
            if line is None:
                line = self._bot.client.recv_timeout(COORDINATION_POLL_TIMEOUT)
            if line is None:
                if not self._bot.client.connected:
                    return State.SURVIVAL
                if needs_food(self._bot.inventory):
                    return State.EATING
                steps += 1
                continue
            if line.startswith("Current level:"):
                return self._apply_level(line)
            if line.strip() == "ko":
                return State.SURVIVAL
            steps += 1
        return State.SURVIVAL

    def _apply_level(self, line: str) -> State:
        """Parse the new level, update bot, exit if max level reached."""
        try:
            new_level = int(line.split(":")[1].strip())
        except (ValueError, IndexError):
            return State.SURVIVAL
        self._bot.level = new_level
        if self._bot.level >= MAX_LEVEL:
            sys.exit(0)
        return State.SURVIVAL
