"""
Unit tests for the main module.
"""
import pytest
from ia.main import parse_arguments, main
from tests.IA.mocks.fake_client import FakeClient
from tests.IA.mocks.fake_bot import FakeBot


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
    Given valid arguments and stubbed client and bot factories
    When main runs
    Then the client connects with the team name, the bot runs and is closed
    """
    created = {}

    def client_factory(host, port):
        created["client"] = FakeClient()
        return created["client"]

    def bot_factory(*a):
        created["bot"] = FakeBot(*a)
        return created["bot"]

    main(["-p", "4242", "-n", "team1", "-h", "localhost"],
         client_factory=client_factory, bot_factory=bot_factory)

    assert created["client"].connect_arg == "team1"
    assert created["bot"].ran is True
    assert created["client"].closed is True


def test_main_connection_error():
    """
    Given a client factory whose connect raises ConnectionError
    When main runs
    Then it exits with status code 1
    """
    def client_factory(host, port):
        return FakeClient(connect_error=ConnectionError("Fail"))

    with pytest.raises(SystemExit) as exc:
        main(argv=["-p", "4242", "-n", "team1"], client_factory=client_factory)
    assert exc.value.code == 1
