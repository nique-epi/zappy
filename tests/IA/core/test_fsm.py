"""Unit tests for the StateMachine."""
# pylint: disable=redefined-outer-name,protected-access,too-few-public-methods
from ia.core.bot import Bot
from ia.core.fsm import StateMachine
from ia.network.client import ZappyClient
from ia.shared.enum import State
from tests.IA.mocks.fake_socket import FakeSocket


class _StubHandler:
    """Records calls and returns a preset next state."""

    def __init__(self, next_state: State):
        self._next_state = next_state
        self.calls = 0

    def handle(self) -> State:
        self.calls += 1
        return self._next_state


class _RecvHandler:
    """Consumes one server line each tick then stays in SURVIVAL."""

    def __init__(self, bot: Bot):
        self._bot = bot

    def handle(self) -> State:
        self._bot.client.recv()
        return State.SURVIVAL


def _make_bot(responses=None) -> Bot:
    """Build a real Bot wired to a FakeSocket with the given responses."""
    fake_socket = FakeSocket([
        r if isinstance(r, bytes) else (r.encode() + b"\n") if r else b""
        for r in (responses or [])
    ])
    zc = ZappyClient("localhost", 4242, sock_factory=lambda: fake_socket)
    zc._sock = fake_socket
    return Bot(10, 10, 1, zc)


def test_state_property_reflects_bot_state():
    """
    Given a StateMachine driving a bot in SURVIVAL
    When the state property is read
    Then it returns the bot's current state
    """
    bot = _make_bot()
    machine = StateMachine(bot)
    assert machine.state == State.SURVIVAL


def test_machine_has_handler_for_every_state():
    """
    Given a freshly built StateMachine
    When its handler table is inspected
    Then every State member has a registered handler
    """
    bot = _make_bot()
    machine = StateMachine(bot)
    for member in State:
        assert member in machine._handlers


def test_tick_dispatches_to_current_state_handler():
    """
    Given a StateMachine whose EXPLORATION handler is a stub
    When the bot is in EXPLORATION and tick runs
    Then that handler is invoked exactly once
    """
    bot = _make_bot()
    bot.state = State.EXPLORATION
    machine = StateMachine(bot)
    stub = _StubHandler(State.EXPLORATION)
    machine._handlers[State.EXPLORATION] = stub
    machine.tick()
    assert stub.calls == 1


def test_tick_applies_returned_transition():
    """
    Given a handler that returns COLLECTING
    When tick runs
    Then the bot transitions to COLLECTING
    """
    bot = _make_bot()
    bot.state = State.EXPLORATION
    machine = StateMachine(bot)
    machine._handlers[State.EXPLORATION] = _StubHandler(State.COLLECTING)
    machine.tick()
    assert bot.state == State.COLLECTING


def test_tick_dispatches_to_new_state_on_next_call():
    """
    Given a tick that moved the bot from EXPLORATION to COLLECTING
    When tick is called again
    Then the COLLECTING handler is dispatched
    """
    bot = _make_bot()
    bot.state = State.EXPLORATION
    machine = StateMachine(bot)
    machine._handlers[State.EXPLORATION] = _StubHandler(State.COLLECTING)
    collecting = _StubHandler(State.SURVIVAL)
    machine._handlers[State.COLLECTING] = collecting
    machine.tick()
    machine.tick()
    assert collecting.calls == 1


def test_handler_instances_persist_between_ticks():
    """
    Given a self-looping stub handler for SURVIVAL
    When tick runs several times
    Then the same instance accumulates the calls
    """
    bot = _make_bot()
    machine = StateMachine(bot)
    stub = _StubHandler(State.SURVIVAL)
    machine._handlers[State.SURVIVAL] = stub
    for _ in range(3):
        machine.tick()
    assert stub.calls == 3


def test_run_stops_when_client_disconnects():
    """
    Given a handler that reads EOF from the server
    When run drives the machine
    Then the loop terminates once the client is disconnected
    """
    bot = _make_bot([b""])
    machine = StateMachine(bot)
    machine._handlers[State.SURVIVAL] = _RecvHandler(bot)
    machine.run()
    assert bot.client.connected is False


def test_run_does_not_tick_when_already_disconnected():
    """
    Given a client closed before the machine runs
    When run is called
    Then no handler is dispatched
    """
    bot = _make_bot()
    bot.client.close()
    machine = StateMachine(bot)
    stub = _StubHandler(State.SURVIVAL)
    machine._handlers[State.SURVIVAL] = stub
    machine.run()
    assert stub.calls == 0


def test_run_stops_on_player_death():
    """
    Given a handler whose recv raises PlayerDeadError
    When run drives the machine
    Then the error is swallowed and run returns cleanly
    """
    bot = _make_bot([b"dead\n"])
    machine = StateMachine(bot)
    machine._handlers[State.SURVIVAL] = _RecvHandler(bot)
    machine.run()
    assert not bot.client._sock._responses
