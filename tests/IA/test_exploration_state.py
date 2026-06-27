"""Unit tests for ExplorationState."""
# pylint: disable=redefined-outer-name,protected-access
from ia.communication.broadcast import (
    MessageType,
    format_message,
    parse_broadcast,
)
from ia.config import (
    EXPLORATION_TURN_INTERVAL,
    FORK_CHECK_INTERVAL,
    FORK_LIMIT,
    FORK_MAX_LEVEL,
    INVENTORY_CHECK_INTERVAL,
    MAX_FORK_DEPTH,
    ROLE_VACANCY_TIMEOUT,
)
from ia.core.bot import Bot
from ia.network.client import ZappyClient
from ia.shared.enum import Direction, Resource, Role, State
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


def test_explore_heads_towards_known_resource_when_mental_map_enabled():
    """
    Given a bot with the mental-map bonus enabled and known linemate to its
    right
    When handle explores with no useful stone in the current Look
    Then it turns Right, the first step of the path to that tile
    """
    bot = _make_bot([_NO_STONES, "ok"])
    bot.mental_map_enabled = True
    bot.inventory[Resource.LINEMATE] = 0
    bot.world_map.update_from_look(
        [{"coords": (3, 0), "objects": ["linemate"]}], bot.turn
    )
    state = ExplorationState(bot)
    state.handle()
    sent = b"".join(bot.client._sock.sent).decode()
    assert "Right" in sent
    assert bot.orientation == Direction.EAST


def test_explore_ignores_known_resource_when_mental_map_disabled():
    """
    Given a bot without the mental-map bonus enabled but a known linemate
    tile recorded to its right
    When handle explores with no useful stone in the current Look
    Then it falls back to the plain wander behaviour (Forward, no turn)
    """
    bot = _make_bot([_NO_STONES, "ok"])
    bot.inventory[Resource.LINEMATE] = 0
    bot.world_map.update_from_look(
        [{"coords": (3, 0), "objects": ["linemate"]}], bot.turn
    )
    state = ExplorationState(bot)
    state.handle()
    sent = b"".join(bot.client._sock.sent).decode()
    assert "Right" not in sent
    assert "Forward" in sent
    assert bot.orientation == Direction.NORTH


def test_explore_updates_bot_position_on_forward():
    """
    Given a bot facing north at the map origin
    When handle explores and sends Forward with no known resource
    Then the bot's position advances and wraps toroidally
    """
    bot = _make_bot([_NO_STONES, "ok"])
    bot.inventory[Resource.LINEMATE] = 1
    bot.fork_count = FORK_LIMIT
    state = ExplorationState(bot)
    state.handle()
    assert bot.pos == (0, 9)


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


def test_self_fork_calls_spawn_player_when_set():
    """
    Given a level-1 bot with spawn_player set and no free Connect_nbr slot
    When the periodic fork check fires during exploration
    Then spawn_player is called exactly once
    """
    bot = _make_bot(["0", "ok", _NO_STONES, "ok"])
    calls = []
    bot.spawn_player = calls.append
    state = ExplorationState(bot)
    state._fork_counter = FORK_CHECK_INTERVAL - 1
    state.handle()
    assert calls == [1]


def test_self_fork_ignores_non_numeric_connect_reply():
    """
    Given a level-1 bot whose Connect_nbr recv returns a non-numeric frame
    When the periodic fork check fires during exploration
    Then it does not crash nor fork and keeps exploring
    """
    bot = _make_bot(["ko", _NO_STONES, "ok"])
    state = ExplorationState(bot)
    state._fork_counter = FORK_CHECK_INTERVAL - 1
    result = state.handle()
    sent = b"".join(bot.client._sock.sent).decode()
    assert "Fork" not in sent
    assert bot.fork_count == 0
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


def test_self_claims_free_slot_without_forking():
    """
    Given a level-1 bot with spawn_player set and a free Connect_nbr slot
    When the periodic fork check fires during exploration
    Then spawn_player is called and no Fork command is sent
    """
    bot = _make_bot(["1", _NO_STONES, "ok"])
    calls = []
    bot.spawn_player = calls.append
    state = ExplorationState(bot)
    state._fork_counter = FORK_CHECK_INTERVAL - 1
    state.handle()
    sent = b"".join(bot.client._sock.sent).decode()
    assert "Fork" not in sent
    assert calls == [1]


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


def test_no_fork_when_depth_limit_reached():
    """
    Given a level-1 bot already at MAX_FORK_DEPTH generations deep
    When the periodic fork check would otherwise fire
    Then Connect_nbr is never queried and no further fork happens
    """
    bot = _make_bot([_NO_STONES, "ok"])
    bot.fork_depth = MAX_FORK_DEPTH
    state = ExplorationState(bot)
    state._fork_counter = FORK_CHECK_INTERVAL - 1
    state.handle()
    sent = b"".join(bot.client._sock.sent).decode()
    assert "Connect_nbr" not in sent
    assert bot.fork_count == 0


def test_spawn_player_receives_incremented_depth():
    """
    Given a bot already two generations deep that needs to fork
    When the periodic fork check forks because no slot is free
    Then spawn_player is called with depth incremented by one
    """
    bot = _make_bot(["0", "ok", _NO_STONES, "ok"])
    bot.fork_depth = 2
    calls = []
    bot.spawn_player = calls.append
    state = ExplorationState(bot)
    state._fork_counter = FORK_CHECK_INTERVAL - 1
    state.handle()
    assert calls == [3]


def test_fork_needed_broadcast_triggers_fork():
    """
    Given an available level-1 bot with no free team slot left
    When a FORK_NEEDED broadcast arrives during exploration
    Then it checks Connect_nbr, forks and resumes exploration
    """
    bot = _make_bot([_fork_needed_line(), "ok", "0", "ok"])
    state = ExplorationState(bot)
    result = state.handle()
    sent = b"".join(bot.client._sock.sent).decode()
    assert "Connect_nbr" in sent
    assert "Fork" in sent
    assert bot.fork_count == 1
    assert result == State.EXPLORATION


def test_fork_needed_broadcast_skipped_when_slot_already_free():
    """
    Given an available level-1 bot with a free team slot remaining
    When a FORK_NEEDED broadcast arrives during exploration
    Then it does not lay a new egg
    """
    bot = _make_bot([_fork_needed_line(), "ok", "1"])
    state = ExplorationState(bot)
    result = state.handle()
    sent = b"".join(bot.client._sock.sent).decode()
    assert "Fork" not in sent
    assert bot.fork_count == 0
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


def _role_claim_line(level: int = 1) -> str:
    """Build a raw ROLE_CLAIM farmer heartbeat line from another player."""
    return (
        f"message 0, {format_message(MessageType.ROLE_CLAIM, level, 'farmer')}"
    )


def test_handle_dispatches_farmer_role_never_collects_stones():
    """
    Given a farmer-role bot seeing a missing stone in its Look
    When handle dispatches to the farmer behaviour
    Then it never takes the stone and stays in EXPLORATION
    """
    bot = _make_bot([_LINEMATE_VISIBLE, "ok"])
    bot.role = Role.FARMER
    bot.inventory[Resource.LINEMATE] = 0
    state = ExplorationState(bot)
    result = state.handle()
    sent = b"".join(bot.client._sock.sent).decode()
    assert result == State.EXPLORATION
    assert "Take" not in sent


def test_collector_promotes_to_farmer_after_vacancy_timeout():
    """
    Given a collector that hears no farmer heartbeat for the timeout window
    When the vacancy is tracked past the threshold
    Then it promotes itself to farmer and broadcasts a ROLE_CLAIM
    """
    bot = _make_bot(["ok"])
    bot.bot_id = 0
    bot.role = Role.COLLECTOR
    state = ExplorationState(bot)
    for _ in range(ROLE_VACANCY_TIMEOUT):
        state.track_farmer_vacancy()
    sent = b"".join(bot.client._sock.sent).decode()
    assert bot.role == Role.FARMER
    assert "ROLE_CLAIM" in sent


def test_collector_resets_vacancy_on_farmer_heartbeat():
    """
    Given a collector with an ongoing farmer-vacancy countdown
    When a ROLE_CLAIM farmer heartbeat is processed
    Then the vacancy counter is reset to zero
    """
    bot = _make_bot([])
    state = ExplorationState(bot)
    state.farmer_vacancy_ticks = 10
    state._handle_broadcast(parse_broadcast(_role_claim_line()))
    assert state.farmer_vacancy_ticks == 0


def test_farmer_role_does_not_promote_on_vacancy_tracking():
    """
    Given a bot already holding the farmer role
    When farmer-vacancy tracking runs repeatedly
    Then it never changes role nor broadcasts
    """
    bot = _make_bot([])
    bot.role = Role.FARMER
    state = ExplorationState(bot)
    for _ in range(ROLE_VACANCY_TIMEOUT + 5):
        state.track_farmer_vacancy()
    sent = b"".join(bot.client._sock.sent).decode()
    assert bot.role == Role.FARMER
    assert sent == ""
