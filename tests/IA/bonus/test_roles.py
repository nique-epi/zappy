"""Unit tests for the role specialisation bonus (ZAP-21)."""
# pylint: disable=redefined-outer-name,protected-access
from ia.bonus import roles
from ia.config import FORK_CHECK_INTERVAL, ROLE_HEARTBEAT_INTERVAL
from ia.core.bot import Bot
from ia.network.client import ZappyClient
from ia.shared.enum import Resource, Role, State
from ia.states.exploration import ExplorationState
from tests.IA.mocks.fake_socket import FakeSocket

_NO_STONES = "[ player,,,,]"
_LINEMATE_VISIBLE = "[ player, linemate]"
_FOOD_ON_TILE = "[ food]"


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


def test_assign_role_last_connector_is_chef():
    """
    Given a connection that reports no free slot left (last connector)
    When assign_role runs
    Then the bot becomes the chef
    """
    assert roles.assign_role(0) == Role.CHEF


def test_assign_role_first_slot_is_collector():
    """
    Given the first connection slot reported by the handshake
    When assign_role runs
    Then the bot becomes a collector
    """
    assert roles.assign_role(1) == Role.COLLECTOR


def test_assign_role_every_third_slot_is_farmer():
    """
    Given a connection slot that is a multiple of the farmer share
    When assign_role runs
    Then the bot becomes a farmer
    """
    assert roles.assign_role(3) == Role.FARMER
    assert roles.assign_role(6) == Role.FARMER


def test_handle_collector_returns_collecting_on_useful_stone():
    """
    Given a collector seeing a missing stone in its Look
    When handle_collector runs
    Then it targets the tile and transitions to COLLECTING
    """
    bot = _make_bot([_LINEMATE_VISIBLE])
    bot.inventory[Resource.LINEMATE] = 0
    explorer = ExplorationState(bot)
    assert roles.handle_collector(explorer) == State.COLLECTING
    assert bot.collect_target == 1


def test_handle_chef_does_not_fork():
    """
    Given a chef whose fork counter is due to fire
    When handle_chef runs
    Then it never queries Connect_nbr nor forks
    """
    bot = _make_bot([_NO_STONES, "ok"])
    bot.inventory[Resource.LINEMATE] = 1
    explorer = ExplorationState(bot)
    explorer._fork_counter = FORK_CHECK_INTERVAL - 1
    result = roles.handle_chef(explorer)
    sent = b"".join(bot.client._sock.sent).decode()
    assert "Connect_nbr" not in sent
    assert "Fork" not in sent
    assert result == State.EXPLORATION


def test_handle_farmer_takes_visible_food():
    """
    Given a farmer with food on its current tile
    When handle_farmer runs
    Then it takes the food and stays in EXPLORATION
    """
    bot = _make_bot([_FOOD_ON_TILE, "ok"])
    explorer = ExplorationState(bot)
    result = roles.handle_farmer(explorer)
    sent = b"".join(bot.client._sock.sent).decode()
    assert "Take food" in sent
    assert result == State.EXPLORATION


def test_handle_farmer_never_takes_stones():
    """
    Given a farmer seeing a stone but no food in its Look
    When handle_farmer runs
    Then it never takes the stone and never enters COLLECTING
    """
    bot = _make_bot([_LINEMATE_VISIBLE, "ok"])
    bot.inventory[Resource.LINEMATE] = 0
    explorer = ExplorationState(bot)
    result = roles.handle_farmer(explorer)
    sent = b"".join(bot.client._sock.sent).decode()
    assert "Take" not in sent
    assert result == State.EXPLORATION


def test_handle_farmer_returns_exploration_on_disconnect():
    """
    Given a farmer whose Look read returns nothing (server closed)
    When handle_farmer runs
    Then it returns EXPLORATION without crashing
    """
    bot = _make_bot([None])
    explorer = ExplorationState(bot)
    assert roles.handle_farmer(explorer) == State.EXPLORATION


def test_farmer_heartbeat_broadcasts_on_interval():
    """
    Given a farmer that has explored a full heartbeat interval
    When the heartbeat fires
    Then a ROLE_CLAIM broadcast announcing the farmer role is sent
    """
    bot = _make_bot(["ok"])
    explorer = ExplorationState(bot)
    for _ in range(ROLE_HEARTBEAT_INTERVAL):
        explorer.maybe_broadcast_farmer_heartbeat()
    sent = b"".join(bot.client._sock.sent).decode()
    assert "ROLE_CLAIM" in sent
    assert "farmer" in sent
