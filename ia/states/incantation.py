"""Incantation state: send Incantation, wait for result, update bot level."""
from ia.config import COORDINATION_POLL_TIMEOUT
from ia.core.bot import Bot
from ia.parsing.inventory import needs_food
from ia.shared.enum import State


class IncantationState:  # pylint: disable=too-few-public-methods
    """Send Incantation as chef or await its result as follower."""

    def __init__(self, bot: Bot) -> None:
        self._bot = bot

    def handle(self) -> State:
        """Send or await Incantation, gated by the critical food threshold."""
        if needs_food(self._bot.inventory):
            return State.EATING

        if self._bot.is_incantation_chef:
            self._bot.client.send("Incantation")
        return self._wait_for_result()

    def _wait_for_result(self) -> State:
        """Read server lines until incantation concludes or timeout."""
        steps = 0
        underway = False
        while steps < 400 or underway:
            line = self._bot.client.pop_notification()
            if line is None:
                line = self._bot.client.recv_timeout(COORDINATION_POLL_TIMEOUT)
            if line is None:
                if not self._bot.client.connected:
                    return State.SURVIVAL
                if not underway and needs_food(self._bot.inventory):
                    return State.EATING
                steps += 1
                continue
            if line.strip() == "Elevation underway":
                underway = True
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
        return State.SURVIVAL
