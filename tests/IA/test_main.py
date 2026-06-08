"""
Unit tests for the main module.
"""
import pytest
from ia.main import parse_arguments, main


class FakeClient:
    """Minimal ZappyClient stand-in for main() tests."""

    def __init__(self, host, port, connect_result=(1, 10, 20), connect_error=None):
        self.host = host
        self.port = port
        self._connect_result = connect_result
        self._connect_error = connect_error
        self.connect_arg = None
        self.closed = False

    def connect(self, team_name):
        self.connect_arg = team_name
        if self._connect_error:
            raise self._connect_error
        return self._connect_result

    def close(self):
        self.closed = True


class FakeBot:
    """Minimal Bot stand-in for main() tests."""

    def __init__(self, width, height, client_num, client):
        self.width = width
        self.height = height
        self.client_num = client_num
        self.client = client
        self.ran = False

    def run(self):
        self.ran = True


def test_parse_arguments_success():
    args = parse_arguments(["-p", "4242", "-n", "team1", "-h", "localhost"])
    assert args.port == 4242
    assert args.name == "team1"
    assert args.host == "localhost"


def test_parse_arguments_invalid_port():
    with pytest.raises(SystemExit):
        parse_arguments(["-p", "70000", "-n", "team1"])


def test_parse_arguments_empty_name():
    with pytest.raises(SystemExit):
        parse_arguments(["-p", "4242", "-n", "  "])


def test_main_success():
    created = {}

    def client_factory(host, port):
        created["client"] = FakeClient(host, port)
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
    def client_factory(host, port):
        return FakeClient(host, port, connect_error=ConnectionError("Fail"))

    with pytest.raises(SystemExit) as exc:
        main(argv=["-p", "4242", "-n", "team1"], client_factory=client_factory)
    assert exc.value.code == 1
