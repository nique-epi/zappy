from ia.core.bot import Bot
from ia.config import FOOD_SAFE_THRESHOLD, MAX_STEPS_WITHOUT_FOOD
from ia.game.navigation import tile_to_moves
from ia.parsing.look import parse_look
from ia.parsing.inventory import parse_inventory
from ia.shared.enum import Resource, State


class EatState:  # pylint: disable=too-few-public-methods
    def __init__(self, bot: Bot):
        self.bot = bot
        self._steps_without_food = 0

    def handle(self) -> State:
        """Check if the bot has enough food; if not, forage for more."""
        if self.bot.inventory.get(Resource.FOOD, 0) >= FOOD_SAFE_THRESHOLD:
            return State.SURVIVAL
        return self._forage()

    def _forage(self) -> State:
        """Look for food in surrounding tiles and move towards it if found."""
        response = self._look()
        if response is None:
            return State.EATING
        tiles = parse_look(
            response, self.bot.pos, self.bot.orientation, self.bot.level
        )
        food_index = next(
            (i for i, tile in enumerate(tiles) if "food" in tile["objects"]),
            None,
        )
        if food_index is not None:
            self._move_and_take(food_index)
        else:
            self._explore()
        return State.EATING

    def _look(self) -> str | None:
        """Send a Look command to the server and return the response."""
        self.bot.client.send("Look")
        return self.bot.client.recv()

    def _move_and_take(self, food_index: int) -> None:
        """Move towards the food and take it."""
        self._steps_without_food = 0
        for move in tile_to_moves(food_index):
            self.bot.client.send(move.value)
            self.bot.client.recv()
        self.bot.client.send("Take food")
        self.bot.client.recv()
        self.bot.client.send("Inventory")
        response = self.bot.client.recv()
        if response is not None:
            self.bot.inventory = parse_inventory(response)

    def _explore(self) -> None:
        """Explore the environment in search of food."""
        self._steps_without_food += 1
        if self._steps_without_food >= MAX_STEPS_WITHOUT_FOOD:
            self._steps_without_food = 0
            self.bot.client.send("Left")
            self.bot.client.recv()
        self.bot.client.send("Forward")
        self.bot.client.recv()
