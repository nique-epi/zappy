"""
Unit tests for the main module.
"""
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
    Given valid arguments and real client and bot factories backed by a FakeSocket
    When main runs
    Then the client connects, the bot runs and the socket is closed
    """
    fake_socket = FakeSocket([b"WELCOME\n", b"1\n", b"10 20\n", b""])
    created = {}

    def client_factory(host, port):
        zc = ZappyClient(host, port, sock_factory=lambda: fake_socket)
        created["client"] = zc
        return zc

    def bot_factory(width, height, client_num, client):
        created["bot"] = Bot(width, height, client_num, client)
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
