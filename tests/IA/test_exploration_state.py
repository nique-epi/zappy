"""Unit tests for ExplorationState."""
# pylint: disable=redefined-outer-name,protected-access
from ia.config import INVENTORY_CHECK_INTERVAL, EXPLORATION_TURN_INTERVAL
from ia.core.bot import Bot
from ia.network.client import ZappyClient
from ia.shared.enum import Resource, State
from ia.states.exploration import ExplorationState
from tests.IA.mocks.fake_socket import FakeSocket

_NO_STONES = "[ player,,,,]"
_LINEMATE_VISIBLE = "[ player, linemate]"


def _make_bot(responses=None) -> Bot:
    """Build a real Bot wired to a FakeSocket with the given responses."""
    fake_socket = FakeSocket([
        r if isinstance(r, bytes)
        else (r.encode() + b"\n") if r else b""
        for r in (responses or [])
    ])
    zc = ZappyClient("localhost", 4242, sock_factory=lambda: fake_socket)
    zc._sock = fake_socket
    return Bot(10, 10, 1, zc)


def test_handle_returns_survival_after_interval():
    """
    Given an ExplorationState called INVENTORY_CHECK_INTERVAL times
    When handle is called once more
    Then State.SURVIVAL is returned to trigger a food check
    """
    responses = [_NO_STONES, "ok"] * INVENTORY_CHECK_INTERVAL
    bot = _make_bot(responses)
    state = ExplorationState(bot)
    result = None
    for _ in range(INVENTORY_CHECK_INTERVAL):
        result = state.handle()
    assert result == State.SURVIVAL


def test_handle_resets_steps_after_survival():
    """
    Given an ExplorationState that just returned SURVIVAL
    When handle is called again immediately after
    Then it does not return SURVIVAL again on the very next call
    """
    responses = [_NO_STONES, "ok"] * (INVENTORY_CHECK_INTERVAL + 2)
    bot = _make_bot(responses)
    state = ExplorationState(bot)
    for _ in range(INVENTORY_CHECK_INTERVAL):
        state.handle()
    result = state.handle()
    assert result != State.SURVIVAL


def test_handle_sends_look_each_step():
    """
    Given an ExplorationState with a bot that has no useful stones nearby
    When handle is called
    Then a Look command is sent to the server
    """
    bot = _make_bot([_NO_STONES, "ok"])
    state = ExplorationState(bot)
    state.handle()
    assert any(b"Look" in s for s in bot.client._sock.sent)


def test_handle_returns_collecting_when_useful_stone_visible():
    """
    Given a bot at level 1 with no linemate and a Look response with linemate
    When handle is called
    Then State.COLLECTING is returned
    """
    bot = _make_bot([_LINEMATE_VISIBLE])
    bot.inventory[Resource.LINEMATE] = 0
    state = ExplorationState(bot)
    assert state.handle() == State.COLLECTING


def test_handle_returns_exploration_when_no_useful_stone():
    """
    Given a bot at level 1 with all required stones already collected
    When handle is called with a Look response showing linemate
    Then State.EXPLORATION is returned since no stone is missing
    """
    bot = _make_bot([_LINEMATE_VISIBLE, "ok"])
    bot.inventory[Resource.LINEMATE] = 1
    state = ExplorationState(bot)
    assert state.handle() == State.EXPLORATION


def test_handle_returns_exploration_when_tiles_empty():
    """
    Given a bot with missing stones but a Look response with no stones visible
    When handle is called
    Then State.EXPLORATION is returned and the bot moves forward
    """
    bot = _make_bot([_NO_STONES, "ok"])
    bot.inventory[Resource.LINEMATE] = 0
    state = ExplorationState(bot)
    result = state.handle()
    assert result == State.EXPLORATION
    sent = b"".join(bot.client._sock.sent).decode()
    assert "Forward" in sent


def test_handle_returns_exploration_when_server_disconnects():
    """
    Given a bot with a server that returns None on Look
    When handle is called
    Then State.EXPLORATION is returned without crashing
    """
    bot = _make_bot([None])
    state = ExplorationState(bot)
    assert state.handle() == State.EXPLORATION


def test_explore_sends_turn_at_turn_interval():
    """
    Given an ExplorationState that has explored EXPLORATION_TURN_INTERVAL steps
    When handle is called on the next step
    Then a turn command (Left or Right) is sent before Forward
    """
    responses = [_NO_STONES, "ok"] * (EXPLORATION_TURN_INTERVAL + 1)
    bot = _make_bot(responses)
    bot.inventory[Resource.LINEMATE] = 1
    state = ExplorationState(bot)
    for _ in range(EXPLORATION_TURN_INTERVAL):
        state.handle()
    state.handle()
    sent = b"".join(bot.client._sock.sent).decode()
    assert "Left" in sent or "Right" in sent


def test_explore_alternates_turn_direction():
    """
    Given an ExplorationState that has already turned Left once
    When enough steps pass for a second turn
    Then a Right command is sent
    """
    steps = EXPLORATION_TURN_INTERVAL * 2 + 2
    responses = [_NO_STONES, "ok"] * steps
    bot = _make_bot(responses)
    bot.inventory[Resource.LINEMATE] = 1
    state = ExplorationState(bot)
    for _ in range(EXPLORATION_TURN_INTERVAL * 2):
        state.handle()
    state.handle()
    sent = b"".join(bot.client._sock.sent).decode()
    assert "Right" in sent


def test_next_exploration_target_returns_none():
    """
    Given an ExplorationState (ZAP-19 not yet implemented)
    When next_exploration_target is called
    Then None is returned
    """
    bot = _make_bot()
    state = ExplorationState(bot)
    assert state.next_exploration_target() is None
