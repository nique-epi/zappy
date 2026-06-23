"""Unit tests for SurvivalState."""
# pylint: disable=redefined-outer-name
from ia.config import FOOD_THRESHOLD, INVENTORY_CHECK_INTERVAL
from ia.core.bot import Bot
from ia.network.client import ZappyClient
from ia.shared.enum import Resource, State
from ia.states.survival import SurvivalState
from tests.IA.mocks.fake_socket import FakeSocket


def _make_bot(responses=None) -> Bot:
    """Build a real Bot wired to a FakeSocket with the given responses."""
    fake_socket = FakeSocket([
        r if isinstance(r, bytes) else (r.encode() + b"\n") if r else b""
        for r in (responses or [])
    ])
    zc = ZappyClient("localhost", 4242, sock_factory=lambda: fake_socket)
    zc._sock = fake_socket
    return Bot(10, 10, 1, zc)


def test_handle_before_interval_returns_current_state():
    """
    Given a SurvivalState and a bot in SURVIVAL state
    When handle is called fewer times than INVENTORY_CHECK_INTERVAL
    Then the current state is returned without sending any command
    """
    bot = _make_bot()
    bot.inventory[Resource.FOOD] = 10
    state = SurvivalState(bot)
    result = None
    for _ in range(INVENTORY_CHECK_INTERVAL - 1):
        result = state.handle()
    assert result == State.SURVIVAL
    assert not bot.client._sock.sent


def test_handle_at_interval_sends_inventory_command():
    """
    Given a SurvivalState and a bot with enough food
    When handle is called exactly INVENTORY_CHECK_INTERVAL times
    Then an Inventory command is sent to the server
    """
    inventory_response = (
        "[food 10, linemate 0, deraumere 0, sibur 0, mendiane 0, phiras 0, thystame 0]"
    )
    bot = _make_bot([inventory_response])
    bot.inventory[Resource.FOOD] = 10
    state = SurvivalState(bot)
    for _ in range(INVENTORY_CHECK_INTERVAL):
        state.handle()
    assert any(b"Inventory" in s for s in bot.client._sock.sent)


def test_handle_returns_eating_when_food_low():
    """
    Given a SurvivalState and a bot whose food is at or below the threshold
    When handle triggers an inventory check
    Then State.EATING is returned
    """
    food_count = FOOD_THRESHOLD
    inventory_response = (
        f"[food {food_count}, linemate 0, deraumere 0, "
        "sibur 0, mendiane 0, phiras 0, thystame 0]"
    )
    bot = _make_bot([inventory_response])
    bot.inventory[Resource.FOOD] = food_count
    state = SurvivalState(bot)
    result = None
    for _ in range(INVENTORY_CHECK_INTERVAL):
        result = state.handle()
    assert result == State.EATING


def test_handle_returns_exploration_when_food_sufficient():
    """
    Given a SurvivalState and a bot with food well above the threshold
    When handle triggers an inventory check
    Then State.EXPLORATION is returned to resume productive work
    """
    food_count = FOOD_THRESHOLD + 10
    inventory_response = (
        f"[food {food_count}, linemate 0, deraumere 0, "
        "sibur 0, mendiane 0, phiras 0, thystame 0]"
    )
    bot = _make_bot([inventory_response])
    bot.inventory[Resource.FOOD] = food_count
    state = SurvivalState(bot)
    result = None
    for _ in range(INVENTORY_CHECK_INTERVAL):
        result = state.handle()
    assert result == State.EXPLORATION


def test_handle_updates_bot_inventory_after_check():
    """
    Given a SurvivalState and a bot with stale inventory
    When handle triggers an inventory check with a new server response
    Then bot.inventory is updated to reflect the server response
    """
    inventory_response = (
        "[food 15, linemate 3, deraumere 0, sibur 0, mendiane 0, phiras 0, thystame 0]"
    )
    bot = _make_bot([inventory_response])
    state = SurvivalState(bot)
    for _ in range(INVENTORY_CHECK_INTERVAL):
        state.handle()
    assert bot.inventory[Resource.FOOD] == 15
    assert bot.inventory[Resource.LINEMATE] == 3


def test_handle_returns_current_state_when_server_disconnects():
    """
    Given a SurvivalState and a bot whose client returns None (disconnected)
    When handle triggers an inventory check
    Then the current bot state is returned without crashing
    """
    bot = _make_bot([None])
    state = SurvivalState(bot)
    result = None
    for _ in range(INVENTORY_CHECK_INTERVAL):
        result = state.handle()
    assert result == State.SURVIVAL


def test_cycle_counter_resets_after_check():
    """
    Given a SurvivalState that has just performed a check
    When handle is called once more immediately after
    Then no new Inventory command is sent (counter was reset)
    """
    inventory_response = (
        "[food 10, linemate 0, deraumere 0, sibur 0, mendiane 0, phiras 0, thystame 0]"
    )
    bot = _make_bot([inventory_response, inventory_response])
    bot.inventory[Resource.FOOD] = 10
    state = SurvivalState(bot)
    for _ in range(INVENTORY_CHECK_INTERVAL):
        state.handle()
    sent_count = len(bot.client._sock.sent)
    state.handle()
    assert len(bot.client._sock.sent) == sent_count
