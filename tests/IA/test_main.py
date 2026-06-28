"""
Unit tests for the main module.
"""
# pylint: disable=protected-access
import pytest
from ia.main import parse_arguments, main
from ia.network.client import ZappyClient
from ia.core.bot import Bot
from tests.IA.mocks.fake_socket import FakeSocket


def test_parse_arguments_success():
    """
    Given a valid set of command line arguments
    When they are parsed
    Then port, name and host are extracted
    """
    args = parse_arguments(["-p", "4242", "-n", "team1", "-h", "localhost"])
    assert args.port == 4242
    assert args.name == "team1"
    assert args.host == "localhost"


def test_parse_arguments_invalid_port():
    """
    Given a port outside the valid range
    When the arguments are parsed
    Then the program exits
    """
    with pytest.raises(SystemExit):
        parse_arguments(["-p", "70000", "-n", "team1"])


def test_parse_arguments_empty_name():
    """
    Given a blank team name
    When the arguments are parsed
    Then the program exits
    """
    with pytest.raises(SystemExit):
        parse_arguments(["-p", "4242", "-n", "  "])


def test_main_success():
    """
    Given valid arguments and real client/bot factories backed by a
    FakeSocket
    When main runs
    Then the client connects, the bot runs and the socket is closed
    """
    fake_socket = FakeSocket([b"WELCOME\n", b"1\n", b"10 20\n", b""])
    created = {}

    def client_factory(host, port):
        zc = ZappyClient(host, port, sock_factory=lambda: fake_socket)
        created["client"] = zc
        return zc

    def bot_factory(width, height, client_num, client, mental_map=False,
                    roles=False):
        created["bot"] = Bot(
            width, height, client_num, client, mental_map=mental_map
        )
        return created["bot"]

    main(["-p", "4242", "-n", "team1", "-h", "localhost"],
         client_factory=client_factory, bot_factory=bot_factory)

    assert created["client"]._sock is None
    assert created["bot"].width == 10
    assert created["bot"].height == 20


def test_main_connection_error():
    """
    Given a client factory whose connect raises ConnectionError
    When main runs
    Then it exits with status code 1
    """
    fake_socket = FakeSocket([b"NOT_WELCOME\n"])

    def client_factory(host, port):
        return ZappyClient(host, port, sock_factory=lambda: fake_socket)

    with pytest.raises(SystemExit) as exc:
        main(argv=["-p", "4242", "-n", "team1"], client_factory=client_factory)
    assert exc.value.code == 1


class _InlineThread:
    """A fake thread that runs its target synchronously on start()."""

    def __init__(self, target, args=()):
        self._target = target
        self._args = args

    def start(self):
        self._target(*self._args)

    def is_alive(self):
        return False

    def join(self, timeout=None):
        pass


def test_main_spawns_second_player_on_fork():
    """
    Given two FakeSockets and an inline thread_factory
    When the first bot's spawn_player callback is invoked
    Then a second client/bot handshake runs via client_factory before
    main returns
    """
    first_socket = FakeSocket([b"WELCOME\n", b"1\n", b"10 20\n", b""])
    second_socket = FakeSocket([b"WELCOME\n", b"2\n", b"10 20\n", b""])
    sockets = [first_socket, second_socket]
    created = []

    def client_factory(host, port):
        return ZappyClient(host, port, sock_factory=lambda: sockets.pop(0))

    def bot_factory(width, height, client_num, client, mental_map=False,
                    roles=False):
        bot = Bot(width, height, client_num, client, mental_map=mental_map)
        created.append(bot)
        return bot

    main(
        ["-p", "4242", "-n", "team1", "-h", "localhost"],
        client_factory=client_factory,
        bot_factory=bot_factory,
        thread_factory=_InlineThread,
    )
    created[0].spawn_player(1)

    assert len(created) == 2
    assert created[1].client_num == 2


def test_main_spawned_player_connection_error_is_swallowed(capsys):
    """
    Given a spawned (non-first) player whose handshake fails
    When its spawn_player callback is invoked
    Then the error is printed but main does not exit the process
    """
    first_socket = FakeSocket([b"WELCOME\n", b"1\n", b"10 20\n", b""])
    second_socket = FakeSocket([b"NOT_WELCOME\n"])
    sockets = [first_socket, second_socket]
    created = []

    def client_factory(host, port):
        return ZappyClient(host, port, sock_factory=lambda: sockets.pop(0))

    def bot_factory(width, height, client_num, client, mental_map=False,
                    roles=False):
        bot = Bot(width, height, client_num, client, mental_map=mental_map)
        created.append(bot)
        return bot

    main(
        ["-p", "4242", "-n", "team1", "-h", "localhost"],
        client_factory=client_factory,
        bot_factory=bot_factory,
        thread_factory=_InlineThread,
    )
    created[0].spawn_player(1)

    assert len(created) == 1
    assert "[ERROR]" in capsys.readouterr().err
