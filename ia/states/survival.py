from ia.config import INVENTORY_CHECK_INTERVAL
from ia.parsing.inventory import needs_food, parse_inventory
from ia.shared.enum import State
from ia.core.bot import Bot


class SurvivalState:
    def __init__(self):
        self._cycles_since_check = 0

    def handle(self, bot: Bot) -> str | State:
        self._cycles_since_check += 1

        if self._cycles_since_check < INVENTORY_CHECK_INTERVAL:
            return bot.state

        bot.client.send("Inventory\n")
        response = bot.client.recv()
        if response is None:
            return bot.state
        self._cycles_since_check = 0
        bot.inventory = parse_inventory(response)

        if needs_food(bot.inventory):
            return State.EATING

        return bot.state
