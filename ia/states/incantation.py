"""Incantation state: send Incantation, wait for result, update bot level."""
import sys

from ia.config import COORDINATION_MAX_WAIT_STEPS, COORDINATION_POLL_TIMEOUT
from ia.core.bot import Bot
from ia.game.elevation import ELEVATION_REQUIREMENTS, MAX_LEVEL
from ia.parsing.inventory import needs_food
from ia.shared.enum import State


class IncantationState:  # pylint: disable=too-few-public-methods
    """Send Incantation as chef or await its result as follower."""

    def __init__(self, bot: Bot) -> None:
        self._bot = bot

    def handle(self) -> State:
        """Send or await Incantation and return the next state."""
        if needs_food(self._bot.inventory):
            return State.EATING

        if self._bot.is_incantation_chef:
            self._set_required_stones()
            self._bot.client.send("Incantation")
        return self._wait_for_result()

    def _set_required_stones(self) -> None:
        """
        Given bot's current level and inventory
        When called before Incantation
        Then places each required stone on the tile with Set
        """
        requirements = ELEVATION_REQUIREMENTS.get(self._bot.level, {})
        for stone, amount in requirements.items():
            if stone == "players":
                continue
            for _ in range(amount):
                self._bot.client.send(f"Set {stone}")
                self._bot.client.recv_ack()

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
