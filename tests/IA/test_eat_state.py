"""Unit tests for EatState."""
# pylint: disable=redefined-outer-name,protected-access
import pytest
from ia.config import FOOD_SAFE_THRESHOLD, MAX_STEPS_WITHOUT_FOOD
from ia.core.bot import Bot
from ia.network.client import ZappyClient
from ia.shared.enum import Resource, State
from ia.states.eat import EatState
from tests.IA.mocks.fake_socket import FakeSocket

_FULL_INVENTORY = (
    "[food 5, linemate 0, deraumere 0,"
    " sibur 0, mendiane 0, phiras 0, thystame 0]"
)


def _make_bot(responses=None) -> Bot:
    """Build a real Bot wired to a FakeSocket with the given responses."""
    fake_socket = FakeSocket([
        r if isinstance(r, bytes) else (r.encode() + b"\n") if r else b""
        for r in (responses or [])
    ])
    zc = ZappyClient("localhost", 4242, sock_factory=lambda: fake_socket)
    zc._sock = fake_socket
    return Bot(10, 10, 1, zc)


@pytest.fixture
def eat_state(bot) -> EatState:
    """Provide a fresh EatState wired to the shared bot fixture."""
    return EatState(bot)


def test_handle_returns_survival_when_food_above_threshold(bot):
    """
    Given a bot whose food inventory is at or above FOOD_SAFE_THRESHOLD
    When handle is called
    Then State.SURVIVAL is returned without sending any command
    """
    bot.inventory[Resource.FOOD] = FOOD_SAFE_THRESHOLD
    state = EatState(bot)
    result = state.handle()
    assert result == State.SURVIVAL
    assert not bot.client._sock.sent


def test_handle_sends_look_when_food_below_threshold():
    """
    Given a bot with insufficient food and a Look response with no food
    When handle is called
    Then a Look command is sent to the server
    """
    bot = _make_bot(["[ player,, linemate,]", "ok"])
    bot.inventory[Resource.FOOD] = 0
    state = EatState(bot)
    state.handle()
    assert any(b"Look" in s for s in bot.client._sock.sent)


def test_handle_returns_eating_when_food_below_threshold():
    """
    Given a bot with insufficient food and a Look response with no food
    When handle is called
    Then State.EATING is returned
    """
    bot = _make_bot(["[ player,, linemate,]", "ok"])
    bot.inventory[Resource.FOOD] = 0
    state = EatState(bot)
    assert state.handle() == State.EATING


def test_handle_moves_and_takes_food_when_visible():
    """
    Given a bot with insufficient food and a Look response containing food
    When handle is called
    Then movement commands and Take food are sent to the server
    """
    bot = _make_bot(["[ player, food]", "ok", "ok", _FULL_INVENTORY])
    bot.inventory[Resource.FOOD] = 0
    state = EatState(bot)
    state.handle()
    sent = b"".join(bot.client._sock.sent).decode()
    assert "Forward" in sent
    assert "Take food" in sent


def test_handle_take_food_updates_inventory():
    """
    Given a bot that finds food on the current tile and takes it
    When handle is called and the server returns an updated inventory
    Then bot.inventory is updated with the new food count
    """
    bot = _make_bot(["[food player]", "ok", _FULL_INVENTORY])
    bot.inventory[Resource.FOOD] = 0
    state = EatState(bot)
    state.handle()
    assert bot.inventory[Resource.FOOD] == 5


def test_handle_sends_forward_when_no_food_visible():
    """
    Given a bot with insufficient food and a Look response with no food
    When handle is called
    Then a Forward command is sent to explore
    """
    bot = _make_bot(["[ player,, linemate,]", "ok"])
    bot.inventory[Resource.FOOD] = 0
    state = EatState(bot)
    state.handle()
    sent = b"".join(bot.client._sock.sent).decode()
    assert "Forward" in sent


def test_handle_sends_left_after_max_steps_without_food():
    """
    Given a bot that explored MAX_STEPS_WITHOUT_FOOD tiles without food
    When handle is called one more time
    Then a Left command is sent before Forward to change direction
    """
    responses = ["[ player,, linemate,]", "ok"] * (MAX_STEPS_WITHOUT_FOOD + 1)
    bot = _make_bot(responses)
    bot.inventory[Resource.FOOD] = 0
    state = EatState(bot)
    for _ in range(MAX_STEPS_WITHOUT_FOOD):
        state.handle()
    state.handle()
    sent = b"".join(bot.client._sock.sent).decode()
    assert "Left" in sent


def test_handle_returns_eating_when_server_disconnects():
    """
    Given a bot with insufficient food and a server that returns None
    When handle is called
    Then State.EATING is returned without crashing
    """
    bot = _make_bot([None])
    bot.inventory[Resource.FOOD] = 0
    state = EatState(bot)
    assert state.handle() == State.EATING


def test_steps_counter_resets_after_finding_food():
    """
    Given a bot that explored several tiles without food then finds food
    When handle processes the food tile
    Then the steps counter is reset to zero
    """
    responses = (
        ["[ player,, linemate,]", "ok"] * 3
        + ["[ player, food]", "ok", "ok", _FULL_INVENTORY]
    )
    bot = _make_bot(responses)
    bot.inventory[Resource.FOOD] = 0
    state = EatState(bot)
    for _ in range(3):
        state.handle()
    state.handle()
    assert state._steps_without_food == 0
