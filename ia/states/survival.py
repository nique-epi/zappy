from ia.config import INVENTORY_CHECK_INTERVAL
from ia.parsing.inventory import needs_food, parse_inventory
from ia.shared.enum import State
from ia.core.bot import Bot


class SurvivalState:  # pylint: disable=too-few-public-methods
    def __init__(self, bot: Bot):
        self.bot = bot
        self._cycles_since_check = 0

    def handle(self) -> State:
        """Periodically check food: EATING if low, EXPLORATION otherwise."""
        self._cycles_since_check += 1

        if self._cycles_since_check < INVENTORY_CHECK_INTERVAL:
            return self.bot.state

        self.bot.client.send("Inventory\n")
        response = self.bot.client.recv()
        if response is None:
            return self.bot.state
        self._cycles_since_check = 0
        self.bot.inventory = parse_inventory(response)

        if needs_food(self.bot.inventory):
            return State.EATING

        return State.EXPLORATION
