"""Unit tests for CoordinationState."""
# pylint: disable=redefined-outer-name,protected-access
from ia.communication.broadcast import format_message, MessageType
from ia.config import (
    COORDINATION_MAX_WAIT_STEPS,
    COORDINATION_REBROADCAST_STEPS,
)
from ia.core.bot import Bot
from ia.network.client import ZappyClient
from ia.shared.enum import Resource, State
from ia.states.coordination import CoordinationState
from tests.IA.mocks.fake_socket import FakeSocket


_FOOD_OK = "[food 10]"


def _broadcast(
    msg_type: MessageType, level: int, direction: int, data: str = ""
) -> str:
    """Build a raw server broadcast line."""
    payload = format_message(msg_type, level, data)
    return f"message {direction}, {payload}"


def _lead_wait_responses(line_value: str, steps: int = None) -> list:
    """Build the exact response sequence _lead consumes while polling.

    handle() consumes one ack for READY and one for the immediate
    FORK_NEEDED broadcast before _lead's loop starts. Every
    COORDINATION_REBROADCAST_STEPS-th step inside the loop also consumes
    a LEAD broadcast ack and a fresh Inventory query, on top of the
    per-step line read.
    """
    steps = COORDINATION_MAX_WAIT_STEPS if steps is None else steps
    responses = ["ok", "ok"]
    for step in range(steps):
        if step % COORDINATION_REBROADCAST_STEPS == 0:
            responses += ["ok", _FOOD_OK]
        responses.append(line_value)
    return responses


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
    bot.inventory[Resource.FOOD] = 10
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
    bot = _make_bot(_lead_wait_responses(_FOOD_OK), level=2)
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
    bot = _make_bot(["ok", "ok", "ok", join_msg], level=2)
    state = CoordinationState(bot)
    result = state.handle()
    assert result == State.INCANTATION
    assert bot.is_incantation_chef is True


def test_chef_requests_fork_immediately_alongside_ready():
    """
    Given a bot at level 2 (requires 2 players)
    When handle starts coordinating
    Then FORK_NEEDED is broadcast right away, before waiting for a quorum
    """
    join_msg = _broadcast(MessageType.JOIN, 2, 0)
    bot = _make_bot(["ok", "ok", "ok", join_msg], level=2)
    state = CoordinationState(bot)
    state.handle()
    sent = b"".join(bot.client._sock.sent).decode()
    assert sent.startswith(
        "Broadcast ZAPPY:READY:2:\nBroadcast ZAPPY:FORK_NEEDED:2:\n"
    )


def test_chef_requests_fork_when_quorum_unreached():
    """
    Given a level-2 chef that times out without gathering enough players
    When handle gives up on the incantation
    Then it broadcasts FORK_NEEDED to grow the team
    """
    bot = _make_bot(_lead_wait_responses(_FOOD_OK), level=2)
    state = CoordinationState(bot)
    result = state.handle()
    sent = b"".join(bot.client._sock.sent).decode()
    assert result == State.EXPLORATION
    assert "ZAPPY:FORK_NEEDED:2:" in sent


def test_chef_ignores_join_from_wrong_level():
    """
    Given a bot at level 2 receiving a JOIN for level 3
    When handle is called
    Then the JOIN is ignored and quorum is not reached
    """
    wrong_join = _broadcast(MessageType.JOIN, 3, 0)
    bot = _make_bot(_lead_wait_responses(wrong_join), level=2)
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
    bot = _make_bot(_lead_wait_responses(distant_join), level=2)
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
        ["ok", "ok", "ok", _FOOD_OK, lead_msg, _FOOD_OK, "ok"], level=2
    )
    state = CoordinationState(bot)
    state.handle()
    sent = b"".join(bot.client._sock.sent).decode()
    assert "Forward" in sent


def test_chef_keeps_priority_over_higher_client_num():
    """
    Given two bots leading simultaneously (this one is client_num=1)
    When a rival LEAD carries a higher client_num (5) as tie-break data
    Then this bot ignores it, keeps leading, and reaches quorum on JOIN
    """
    rival_lead = _broadcast(MessageType.LEAD, 2, 1, data="5")
    join_msg = _broadcast(MessageType.JOIN, 2, 0)
    bot = _make_bot(
        ["ok", "ok", "ok", _FOOD_OK, rival_lead, join_msg], level=2
    )
    state = CoordinationState(bot)
    result = state.handle()
    sent = b"".join(bot.client._sock.sent).decode()
    assert result == State.INCANTATION
    assert bot.is_incantation_chef is True
    assert "Forward" not in sent


def test_chef_yields_to_lower_client_num():
    """
    Given two bots leading simultaneously (this one is client_num=1)
    When a rival LEAD carries a lower client_num (0) as tie-break data
    Then this bot yields and switches to follower mode (moves Forward)
    """
    rival_lead = _broadcast(MessageType.LEAD, 2, 1, data="0")
    bot = _make_bot(
        ["ok", "ok", "ok", _FOOD_OK, rival_lead, _FOOD_OK, "ok"], level=2
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
    bot = _make_bot(["ok", "ok", "ok"], level=2)
    state = CoordinationState(bot)
    assert state.handle() == State.EXPLORATION


def test_chef_returns_exploration_when_timeout_expires():
    """
    Given a bot at level 2 that receives only noise responses
    When handle times out without reaching quorum
    Then State.EXPLORATION is returned
    """
    bot = _make_bot(_lead_wait_responses(_FOOD_OK), level=2)
    state = CoordinationState(bot)
    assert state.handle() == State.EXPLORATION


def test_follower_ignores_stale_self_echo_after_yielding():
    """
    Given our own LEAD echo (direction 0) queued right behind a rival's
    LEAD (direction 2) during the same ack wait
    When this bot yields to the rival and switches to follower mode
    Then the stale self-echo is discarded instead of being read as
    "the rival is on my tile", so the bot actually moves toward it
    """
    rival_lead = _broadcast(MessageType.LEAD, 2, 2, data="0")
    own_echo = _broadcast(MessageType.LEAD, 2, 0, data="1")
    bot = _make_bot(
        [
            "ok", "ok", rival_lead, own_echo, "ok",
            _FOOD_OK, _FOOD_OK, "ok", "ok",
        ],
        level=2,
    )
    state = CoordinationState(bot)
    state.handle()
    sent = b"".join(bot.client._sock.sent).decode()
    assert "Right" in sent
    assert "Forward" in sent
    assert "ZAPPY:JOIN" not in sent


def test_follower_moves_toward_direction():
    """
    Given a bot at level 2 that receives a LEAD with direction 3 (right)
    When handle switches to follower mode
    Then Right is sent toward the chef
    """
    lead_from_side = _broadcast(MessageType.LEAD, 2, 3)
    bot = _make_bot(
        ["ok", "ok", "ok", _FOOD_OK, lead_from_side, _FOOD_OK, "ok", "ok"],
        level=2,
    )
    state = CoordinationState(bot)
    state.handle()
    sent = b"".join(bot.client._sock.sent).decode()
    assert "Right" in sent


def test_chef_ignores_own_lead_echo():
    """
    Given a chef that hears its own LEAD broadcast echoed back (direction 0)
    When handle processes that echo
    Then it keeps leading instead of deferring to itself
    """
    own_lead_echo = _broadcast(MessageType.LEAD, 2, 0)
    join_msg = _broadcast(MessageType.JOIN, 2, 0)
    bot = _make_bot(
        ["ok", "ok", "ok", _FOOD_OK, own_lead_echo, "ok", join_msg],
        level=2,
    )
    state = CoordinationState(bot)
    result = state.handle()
    assert result == State.INCANTATION
    assert bot.is_incantation_chef is True


def test_follower_transitions_to_incantation_after_join():
    """
    Given a follower that reaches the chef's tile (direction 0)
    When handle broadcasts JOIN
    Then it becomes follower and transitions to INCANTATION once
    """
    lead_other_player = _broadcast(MessageType.LEAD, 2, 1)
    lead_same_tile = _broadcast(MessageType.LEAD, 2, 0)
    bot = _make_bot(
        [
            "ok", "ok", "ok", _FOOD_OK, lead_other_player,
            _FOOD_OK, "ok", lead_same_tile, "ok",
        ],
        level=2,
    )
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
            "ok", "ok", "ok", _FOOD_OK, lead_forward,
            _FOOD_OK, "ok", lead_arrived, "ok",
        ],
        level=2,
    )
    state = CoordinationState(bot)
    state.handle()
    sent = b"".join(bot.client._sock.sent).decode()
    assert "ZAPPY:JOIN:2:" in sent
