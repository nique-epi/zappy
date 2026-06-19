"""Unit tests for CoordinationState."""
# pylint: disable=redefined-outer-name,protected-access
from ia.communication.broadcast import format_message, MessageType
from ia.core.bot import Bot
from ia.network.client import ZappyClient
from ia.shared.enum import Resource, State
from ia.states.coordination import CoordinationState
from tests.IA.mocks.fake_socket import FakeSocket


def _broadcast(msg_type: MessageType, level: int, direction: int) -> str:
    """Build a raw server broadcast line."""
    payload = format_message(msg_type, level)
    return f"message {direction}, {payload}"


def _make_bot(responses: list, level: int = 1) -> Bot:
    """Build a Bot wired to a FakeSocket with preset server responses."""
    encoded = [
        r if isinstance(r, bytes)
        else (r.encode() + b"\n") if r else b""
        for r in responses
    ]
    fake_socket = FakeSocket(encoded)
    zc = ZappyClient("localhost", 4242, sock_factory=lambda: fake_socket)
    zc._sock = fake_socket
    bot = Bot(10, 10, 1, zc)
    bot.level = level
    bot.inventory = dict.fromkeys(Resource, 0)
    return bot


def test_level1_solo_transitions_to_incantation():
    """
    Given a bot at level 1 (requires 1 player)
    When handle is called
    Then it becomes chef and transitions to INCANTATION without broadcasting
    """
    bot = _make_bot([])
    state = CoordinationState(bot)
    result = state.handle()
    sent = b"".join(bot.client._sock.sent).decode()
    assert result == State.INCANTATION
    assert bot.is_incantation_chef is True
    assert "Broadcast" not in sent


def test_chef_broadcasts_ready_then_lead():
    """
    Given a bot at level 2 (requires 2 players) with no incoming messages
    When handle is called and wait steps are exhausted
    Then READY and LEAD are broadcast before abandoning
    """
    bot = _make_bot(
        ["ok", "ok"] + ["irrelevant"] * 40 + ["ok"],
        level=2,
    )
    state = CoordinationState(bot)
    state.handle()
    sent = b"".join(bot.client._sock.sent).decode()
    assert "ZAPPY:READY:2:" in sent
    assert "ZAPPY:LEAD:2:" in sent


def test_chef_transitions_to_incantation_on_quorum():
    """
    Given a bot at level 2 (requires 2 players) and one JOIN with direction 0
    When handle reaches quorum
    Then it becomes chef and transitions to INCANTATION
    """
    join_msg = _broadcast(MessageType.JOIN, 2, 0)
    bot = _make_bot(["ok", "ok", join_msg], level=2)
    state = CoordinationState(bot)
    result = state.handle()
    assert result == State.INCANTATION
    assert bot.is_incantation_chef is True


def test_chef_ignores_join_from_wrong_level():
    """
    Given a bot at level 2 receiving a JOIN for level 3
    When handle is called
    Then the JOIN is ignored and quorum is not reached
    """
    wrong_join = _broadcast(MessageType.JOIN, 3, 0)
    bot = _make_bot(
        ["ok", "ok"] + [wrong_join] * 5 + ["ok"] * 40,
        level=2,
    )
    state = CoordinationState(bot)
    result = state.handle()
    assert result == State.EXPLORATION


def test_chef_ignores_join_from_nonzero_direction():
    """
    Given a bot at level 2 receiving a JOIN with direction 3 (not same tile)
    When handle is called
    Then the JOIN is ignored and quorum is not reached
    """
    distant_join = _broadcast(MessageType.JOIN, 2, 3)
    bot = _make_bot(
        ["ok", "ok"] + [distant_join] * 5 + ["ok"] * 40,
        level=2,
    )
    state = CoordinationState(bot)
    result = state.handle()
    assert result == State.EXPLORATION


def test_chef_defers_to_other_lead():
    """
    Given a bot at level 2 receiving a LEAD from another bot at direction 1
    When handle is called
    Then the bot switches to follower mode (moves Forward)
    """
    lead_msg = _broadcast(MessageType.LEAD, 2, 1)
    bot = _make_bot(
        ["ok", "ok", lead_msg, "ok", "Current level: 3"],
        level=2,
    )
    state = CoordinationState(bot)
    state.handle()
    sent = b"".join(bot.client._sock.sent).decode()
    assert "Forward" in sent


def test_chef_returns_exploration_on_disconnect_while_waiting():
    """
    Given a bot at level 2 whose connection drops while waiting for followers
    When handle is called
    Then State.EXPLORATION is returned
    """
    bot = _make_bot(["ok", "ok"], level=2)
    state = CoordinationState(bot)
    assert state.handle() == State.EXPLORATION


def test_chef_returns_exploration_when_timeout_expires():
    """
    Given a bot at level 2 that receives only noise responses
    When handle times out without reaching quorum
    Then State.EXPLORATION is returned
    """
    bot = _make_bot(["ok"] * 100, level=2)
    state = CoordinationState(bot)
    assert state.handle() == State.EXPLORATION


def test_follower_moves_toward_direction():
    """
    Given a bot at level 2 that receives a LEAD with direction 3 (right)
    When handle switches to follower mode
    Then Right is sent toward the chef
    """
    lead_from_side = _broadcast(MessageType.LEAD, 2, 3)
    bot = _make_bot(
        ["ok", "ok", lead_from_side, "ok", "Current level: 3"],
        level=2,
    )
    state = CoordinationState(bot)
    state.handle()
    sent = b"".join(bot.client._sock.sent).decode()
    assert "Right" in sent


def test_follower_broadcasts_join_when_on_same_tile():
    """
    Given a follower that receives LEAD with direction 0 (same tile)
    When _follow is called with direction 0 initially
    Then JOIN is broadcast before awaiting incantation
    """
    lead_same_tile = _broadcast(MessageType.LEAD, 2, 0)
    bot = _make_bot(
        ["ok", "ok", lead_same_tile, "ok", "Current level: 3"],
        level=2,
    )
    state = CoordinationState(bot)
    state.handle()
    sent = b"".join(bot.client._sock.sent).decode()
    assert "ZAPPY:JOIN:2:" in sent


def test_follower_transitions_to_incantation_after_join():
    """
    Given a follower that reaches the chef's tile (direction 0)
    When handle broadcasts JOIN
    Then it becomes follower and transitions to INCANTATION once
    """
    lead_same_tile = _broadcast(MessageType.LEAD, 2, 0)
    bot = _make_bot(["ok", "ok", lead_same_tile, "ok"], level=2)
    state = CoordinationState(bot)
    result = state.handle()
    sent = b"".join(bot.client._sock.sent).decode()
    assert result == State.INCANTATION
    assert bot.is_incantation_chef is False
    assert sent.count("ZAPPY:JOIN:2:") == 1


def test_follower_updates_direction_on_new_lead():
    """
    Given a follower that first receives LEAD from direction 1 then 0
    When handle follows those broadcasts
    Then bot eventually broadcasts JOIN (direction reached 0)
    """
    lead_forward = _broadcast(MessageType.LEAD, 2, 1)
    lead_arrived = _broadcast(MessageType.LEAD, 2, 0)
    bot = _make_bot(
        [
            "ok", "ok",
            lead_forward,
            "ok",
            lead_arrived,
            "ok",
            "Current level: 3",
        ],
        level=2,
    )
    state = CoordinationState(bot)
    state.handle()
    sent = b"".join(bot.client._sock.sent).decode()
    assert "ZAPPY:JOIN:2:" in sent
