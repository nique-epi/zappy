"""Unit tests for ExplorationState."""
# pylint: disable=redefined-outer-name,protected-access
from ia.communication.broadcast import MessageType, format_message
from ia.config import (
    EXPLORATION_TURN_INTERVAL,
    FORK_CHECK_INTERVAL,
    FORK_LIMIT,
    FORK_MAX_LEVEL,
    INVENTORY_CHECK_INTERVAL,
)
from ia.core.bot import Bot
from ia.network.client import ZappyClient
from ia.shared.enum import Resource, State
from ia.states.exploration import ExplorationState
from tests.IA.mocks.fake_socket import FakeSocket

_NO_STONES = "[ player,,,,]"
_LINEMATE_VISIBLE = "[ player, linemate]"


def _fork_needed_line(level: int = 1) -> str:
    """Build a raw FORK_NEEDED broadcast line from another player."""
    return f"message 0, {format_message(MessageType.FORK_NEEDED, level)}"


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
    bot.fork_count = FORK_LIMIT
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
    bot.fork_count = FORK_LIMIT
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


def test_handle_sets_collect_target_when_useful_stone_visible():
    """
    Given a bot at level 1 with no linemate and linemate visible on tile 1
    When handle is called and State.COLLECTING is returned
    Then bot.collect_target is set to the index of the tile containing linemate
    """
    bot = _make_bot([_LINEMATE_VISIBLE])
    bot.inventory[Resource.LINEMATE] = 0
    state = ExplorationState(bot)
    state.handle()
    assert bot.collect_target == 1


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
    bot.fork_count = FORK_LIMIT
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
    bot.fork_count = FORK_LIMIT
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


def test_self_fork_when_no_free_slot():
    """
    Given a level-1 bot whose Connect_nbr reports no free slot
    When the periodic fork check fires during exploration
    Then a Fork command is sent and fork_count is incremented
    """
    bot = _make_bot(["0", "ok", _NO_STONES, "ok"])
    state = ExplorationState(bot)
    state._fork_counter = FORK_CHECK_INTERVAL - 1
    result = state.handle()
    sent = b"".join(bot.client._sock.sent).decode()
    assert "Fork" in sent
    assert bot.fork_count == 1
    assert result == State.EXPLORATION


def test_no_fork_when_free_slot_available():
    """
    Given a level-1 bot whose Connect_nbr reports a free slot
    When the periodic fork check fires during exploration
    Then no Fork command is sent
    """
    bot = _make_bot(["2", _NO_STONES, "ok"])
    state = ExplorationState(bot)
    state._fork_counter = FORK_CHECK_INTERVAL - 1
    state.handle()
    sent = b"".join(bot.client._sock.sent).decode()
    assert "Fork" not in sent
    assert bot.fork_count == 0


def test_no_fork_check_when_level_too_high():
    """
    Given a bot above FORK_MAX_LEVEL
    When the periodic fork check would otherwise fire
    Then Connect_nbr is never queried and no fork happens
    """
    bot = _make_bot([_NO_STONES, "ok"])
    bot.level = FORK_MAX_LEVEL + 1
    state = ExplorationState(bot)
    state._fork_counter = FORK_CHECK_INTERVAL - 1
    state.handle()
    sent = b"".join(bot.client._sock.sent).decode()
    assert "Connect_nbr" not in sent
    assert bot.fork_count == 0


def test_no_fork_when_limit_reached():
    """
    Given a level-1 bot that already forked FORK_LIMIT times
    When the periodic fork check would otherwise fire
    Then Connect_nbr is never queried and no further fork happens
    """
    bot = _make_bot([_NO_STONES, "ok"])
    bot.fork_count = FORK_LIMIT
    state = ExplorationState(bot)
    state._fork_counter = FORK_CHECK_INTERVAL - 1
    state.handle()
    sent = b"".join(bot.client._sock.sent).decode()
    assert "Connect_nbr" not in sent
    assert bot.fork_count == FORK_LIMIT


def test_fork_needed_broadcast_triggers_fork():
    """
    Given an available level-1 bot
    When a FORK_NEEDED broadcast arrives during exploration
    Then it forks and resumes exploration without interruption
    """
    bot = _make_bot([_fork_needed_line(), "ok"])
    state = ExplorationState(bot)
    result = state.handle()
    sent = b"".join(bot.client._sock.sent).decode()
    assert "Fork" in sent
    assert bot.fork_count == 1
    assert result == State.EXPLORATION


def test_fork_needed_broadcast_ignored_when_unavailable():
    """
    Given a bot above FORK_MAX_LEVEL
    When a FORK_NEEDED broadcast arrives during exploration
    Then it does not fork
    """
    bot = _make_bot([_fork_needed_line()])
    bot.level = FORK_MAX_LEVEL + 1
    state = ExplorationState(bot)
    result = state.handle()
    sent = b"".join(bot.client._sock.sent).decode()
    assert "Fork" not in sent
    assert bot.fork_count == 0
    assert result == State.EXPLORATION
