"""Unit tests for CollectState."""
# pylint: disable=redefined-outer-name,protected-access
from ia.core.bot import Bot
from ia.network.client import ZappyClient
from ia.shared.enum import Resource, State
from ia.states.collect import CollectState
from tests.IA.mocks.fake_socket import FakeSocket

_LOOK_LINEMATE = "[linemate player]"
_LOOK_EMPTY = "[ player,,]"
_LOOK_MULTI = "[linemate linemate player]"


def _make_bot(responses=None, target=0) -> Bot:
    """Build a real Bot wired to a FakeSocket with the given responses."""
    fake_socket = FakeSocket([
        r if isinstance(r, bytes)
        else (r.encode() + b"\n") if r else b""
        for r in (responses or [])
    ])
    zc = ZappyClient("localhost", 4242, sock_factory=lambda: fake_socket)
    zc._sock = fake_socket
    bot = Bot(10, 10, 1, zc)
    bot.collect_target = target
    return bot


def test_handle_moves_to_target_tile():
    """
    Given a bot whose collect_target is 0 (current tile)
    When handle is called
    Then no Forward command is sent before Eject/Look
    """
    bot = _make_bot(["ok", _LOOK_EMPTY], target=0)
    state = CollectState(bot)
    state.handle()
    sent = b"".join(bot.client._sock.sent).decode()
    assert "Forward" not in sent
    assert sent.startswith("Eject")


def test_handle_sends_forward_to_reach_adjacent_tile():
    """
    Given a bot whose collect_target is 1 (one tile ahead)
    When handle is called
    Then a Forward command is sent before Look
    """
    bot = _make_bot(["ok", "ok", "ok", "ok", _LOOK_EMPTY], target=1)
    state = CollectState(bot)
    state.handle()
    sent = b"".join(bot.client._sock.sent).decode()
    assert "Forward" in sent


def test_handle_takes_useful_stone_on_tile():
    """
    Given a bot missing linemate and a tile containing linemate
    When handle is called with collect_target 0
    Then Take linemate is sent to the server
    """
    bot = _make_bot(["ok", _LOOK_LINEMATE, "ok", "ok"], target=0)
    bot.inventory[Resource.LINEMATE] = 0
    state = CollectState(bot)
    state.handle()
    sent = b"".join(bot.client._sock.sent).decode()
    assert "Take linemate" in sent


def test_handle_updates_inventory_on_ok():
    """
    Given a bot at level 2 missing only linemate and a server responding ok
    When handle is called
    Then bot.inventory[LINEMATE] is incremented and not yet dropped, since
    deraumere/sibur are still missing
    """
    bot = _make_bot(["ok", _LOOK_LINEMATE, "ok"], target=0)
    bot.level = 2
    bot.inventory[Resource.LINEMATE] = 0
    state = CollectState(bot)
    state.handle()
    assert bot.inventory[Resource.LINEMATE] == 1


def test_handle_does_not_update_inventory_on_ko():
    """
    Given a bot missing linemate and a server responding ko to Take
    When handle is called
    Then bot.inventory[LINEMATE] remains unchanged
    """
    bot = _make_bot([_LOOK_LINEMATE, "ko"], target=0)
    bot.inventory[Resource.LINEMATE] = 0
    state = CollectState(bot)
    state.handle()
    assert bot.inventory[Resource.LINEMATE] == 0


def test_handle_returns_coordination_when_all_stones_collected():
    """
    Given a bot that collects the last missing stone
    When handle is called and Take returns ok
    Then State.COORDINATION is returned
    """
    bot = _make_bot(["ok", _LOOK_LINEMATE, "ok", "ok"], target=0)
    bot.inventory[Resource.LINEMATE] = 0
    state = CollectState(bot)
    assert state.handle() == State.COORDINATION


def test_handle_returns_exploration_when_stones_still_missing():
    """
    Given a bot at level 1 that already has linemate but tile has no stone
    When handle is called
    Then State.EXPLORATION is returned
    """
    bot = _make_bot([_LOOK_EMPTY], target=0)
    bot.inventory[Resource.LINEMATE] = 0
    state = CollectState(bot)
    assert state.handle() == State.EXPLORATION


def test_handle_takes_multiple_stones_on_same_tile():
    """
    Given a tile with two linemate and a bot missing both
    When handle is called
    Then two Take linemate commands are sent
    """
    bot = _make_bot(["ok", _LOOK_MULTI, "ok", "ok"], target=0)
    bot.inventory[Resource.LINEMATE] = 0
    state = CollectState(bot)
    state.handle()
    sent = b"".join(bot.client._sock.sent).decode()
    assert sent.count("Take linemate") == 1


def test_handle_skips_stone_not_in_missing():
    """
    Given a bot that already has all required stones but tile shows linemate
    When handle is called
    Then no Take command is sent
    """
    bot = _make_bot(["ok", _LOOK_LINEMATE, "ok"], target=0)
    bot.inventory[Resource.LINEMATE] = 1
    state = CollectState(bot)
    state.handle()
    sent = b"".join(bot.client._sock.sent).decode()
    assert "Take" not in sent


def test_handle_returns_exploration_when_server_disconnects():
    """
    Given a server that returns None on Look after moving
    When handle is called
    Then State.EXPLORATION is returned without crashing
    """
    bot = _make_bot([None], target=0)
    state = CollectState(bot)
    assert state.handle() == State.EXPLORATION


def test_handle_returns_exploration_when_move_fails():
    """
    Given a server that returns ko on a Forward move
    When handle is called with a non-zero collect_target
    Then State.EXPLORATION is returned without sending Look or Take
    """
    bot = _make_bot(["ok", "ko"], target=1)
    state = CollectState(bot)
    result = state.handle()
    sent = b"".join(bot.client._sock.sent).decode()
    assert result == State.EXPLORATION
    assert "Look" not in sent
    assert "Take" not in sent


def test_handle_returns_exploration_when_move_returns_none():
    """
    Given a server that returns None during movement
    When handle is called with a non-zero collect_target
    Then State.EXPLORATION is returned without sending Look or Take
    """
    bot = _make_bot([None], target=1)
    state = CollectState(bot)
    result = state.handle()
    sent = b"".join(bot.client._sock.sent).decode()
    assert result == State.EXPLORATION
    assert "Look" not in sent
