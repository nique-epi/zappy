"""Central finite state machine orchestrating the bot behaviours."""
from ia.core.bot import Bot
from ia.network.exceptions import PlayerDeadError
from ia.shared.enum import State
from ia.states.collect import CollectState
from ia.states.coordination import CoordinationState
from ia.states.eat import EatState
from ia.states.exploration import ExplorationState
from ia.states.incantation import IncantationState
from ia.states.survival import SurvivalState


class StateMachine:
    """Dispatch each tick to the handler of the bot's current state."""

    def __init__(self, bot: Bot) -> None:
        self._bot = bot
        self._handlers = {
            State.SURVIVAL: SurvivalState(bot),
            State.EATING: EatState(bot),
            State.EXPLORATION: ExplorationState(bot),
            State.COLLECTING: CollectState(bot),
            State.COORDINATION: CoordinationState(bot),
            State.INCANTATION: IncantationState(bot),
        }

    @property
    def state(self) -> State:
        """Current state of the driven bot."""
        return self._bot.state

    def tick(self) -> None:
        """Run the current state handler and apply the returned transition."""
        self._bot.turn += 1
        handler = self._handlers[self._bot.state]
        self._bot.state = handler.handle()

    def run(self) -> None:
        """Drive ticks until the player dies or the server closes the link."""
        try:
            while self._bot.client.connected:
                self.tick()
        except PlayerDeadError:
            pass
