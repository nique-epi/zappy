"""Unit tests for SurvivalState."""
# pylint: disable=redefined-outer-name
import pytest
from ia.config import FOOD_THRESHOLD, INVENTORY_CHECK_INTERVAL
from ia.shared.enum import Resource, State
from ia.states.survival import SurvivalState
from tests.IA.mocks.fake_bot import FakeBot
from tests.IA.mocks.fake_client import FakeClient


@pytest.fixture
def state():
    """Provide a fresh SurvivalState for each test."""
    return SurvivalState()


def test_handle_before_interval_returns_current_state(state):
    """
    Given a SurvivalState and a bot in SURVIVAL state
    When handle is called fewer times than INVENTORY_CHECK_INTERVAL
    Then the current state is returned without sending any command
    """
    bot = FakeBot()
    bot.inventory[Resource.FOOD] = 10
    for _ in range(INVENTORY_CHECK_INTERVAL - 1):
        result = state.handle(bot)
    assert result == State.SURVIVAL
    assert not bot.client.sent


def test_handle_at_interval_sends_inventory_command(state):
    """
    Given a SurvivalState and a bot with enough food
    When handle is called exactly INVENTORY_CHECK_INTERVAL times
    Then an Inventory command is sent to the server
    """
    inventory_response = (
        "[food 10, linemate 0, deraumere 0, sibur 0, mendiane 0, phiras 0, thystame 0]"
    )
    bot = FakeBot(client=FakeClient([inventory_response]))
    bot.inventory[Resource.FOOD] = 10
    for _ in range(INVENTORY_CHECK_INTERVAL):
        state.handle(bot)
    assert "Inventory\n" in bot.client.sent


def test_handle_returns_eating_when_food_low(state):
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
    bot = FakeBot(client=FakeClient([inventory_response]))
    bot.inventory[Resource.FOOD] = food_count
    result = None
    for _ in range(INVENTORY_CHECK_INTERVAL):
        result = state.handle(bot)
    assert result == State.EATING


def test_handle_returns_current_state_when_food_sufficient(state):
    """
    Given a SurvivalState and a bot with food well above the threshold
    When handle triggers an inventory check
    Then the current bot state is returned unchanged
    """
    food_count = FOOD_THRESHOLD + 10
    inventory_response = (
        f"[food {food_count}, linemate 0, deraumere 0, "
        "sibur 0, mendiane 0, phiras 0, thystame 0]"
    )
    bot = FakeBot(client=FakeClient([inventory_response]))
    bot.inventory[Resource.FOOD] = food_count
    result = None
    for _ in range(INVENTORY_CHECK_INTERVAL):
        result = state.handle(bot)
    assert result == State.SURVIVAL


def test_handle_updates_bot_inventory_after_check(state):
    """
    Given a SurvivalState and a bot with stale inventory
    When handle triggers an inventory check with a new server response
    Then bot.inventory is updated to reflect the server response
    """
    inventory_response = (
        "[food 15, linemate 3, deraumere 0, sibur 0, mendiane 0, phiras 0, thystame 0]"
    )
    bot = FakeBot(client=FakeClient([inventory_response]))
    for _ in range(INVENTORY_CHECK_INTERVAL):
        state.handle(bot)
    assert bot.inventory[Resource.FOOD] == 15
    assert bot.inventory[Resource.LINEMATE] == 3


def test_handle_returns_current_state_when_server_disconnects(state):
    """
    Given a SurvivalState and a bot whose client returns None (disconnected)
    When handle triggers an inventory check
    Then the current bot state is returned without crashing
    """
    bot = FakeBot(client=FakeClient([None]))
    result = None
    for _ in range(INVENTORY_CHECK_INTERVAL):
        result = state.handle(bot)
    assert result == State.SURVIVAL


def test_cycle_counter_resets_after_check(state):
    """
    Given a SurvivalState that has just performed a check
    When handle is called once more immediately after
    Then no new Inventory command is sent (counter was reset)
    """
    inventory_response = (
        "[food 10, linemate 0, deraumere 0, sibur 0, mendiane 0, phiras 0, thystame 0]"
    )
    bot = FakeBot(client=FakeClient([inventory_response, inventory_response]))
    bot.inventory[Resource.FOOD] = 10
    for _ in range(INVENTORY_CHECK_INTERVAL):
        state.handle(bot)
    sent_count = len(bot.client.sent)
    state.handle(bot)
    assert len(bot.client.sent) == sent_count
