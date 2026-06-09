"""
Unit tests for the Bot class.
"""
import pytest
from ia.core.bot import Bot
from ia.network.exceptions import PlayerDeadError


class FakeClient:
    """Minimal ZappyClient stand-in for unit tests."""

    def __init__(self, responses=None):
        self._responses = list(responses or [])
        self.sent = []
        self.recv_calls = 0

    def recv(self):
        self.recv_calls += 1
        if not self._responses:
            return None
        item = self._responses.pop(0)
        if isinstance(item, Exception):
            raise item
        return item

    def send(self, command):
        self.sent.append(command)

    def close(self):
        pass


@pytest.fixture
def client():
    return FakeClient()


def test_bot_init(client):
    """
    Given a world size, a connection slot and a client
    When a Bot is created
    Then its dimensions, level, role and empty map are initialised
    """
    bot = Bot(10, 20, 1, client)
    assert bot.width == 10
    assert bot.height == 20
    assert bot.client_num == 1
    assert bot.level == 1
    assert bot.role == "generic"
    assert len(bot.map) == 20
    assert len(bot.map[0]) == 10


def test_bot_assign_role(client):
    """
    Given a bot built from a connection slot
    When the role is assigned
    Then it defaults to the generic role
    """
    bot = Bot(10, 20, 1, client)
    assert bot._assign_role(1) == "generic"


def test_bot_run_exit_on_none(client):
    """
    Given a client whose recv immediately returns None
    When the bot runs its loop
    Then it stops after a single recv
    """
    bot = Bot(10, 20, 1, client)
    bot.run()
    assert client.recv_calls == 1


def test_bot_run_processes_lines_then_exits(client):
    """
    Given a client returning a couple of lines then a closed connection
    When the bot runs its loop
    Then all lines are consumed and recv is called until None is returned
    """
    client._responses = ["ok", "ko"]
    bot = Bot(10, 20, 1, client)

    bot.run()

    assert client.recv_calls == 3


def test_bot_run_exit_on_dead():
    """
    Given a client whose recv raises PlayerDeadError
    When the bot runs its loop
    Then it exits cleanly without propagating the error
    """
    client = FakeClient([PlayerDeadError()])
    bot = Bot(10, 20, 1, client)
    bot.run()
    assert client.recv_calls == 1


def test_bot_inventory_initialised_to_zeros(client):
    """
    Given a freshly created Bot
    When its inventory attribute is inspected
    Then every resource is set to 0
    """
    from ia.shared.enum import RESOURCES
    bot = Bot(10, 20, 1, client)
    assert bot.inventory == {resource: 0 for resource in RESOURCES}


def test_bot_handle_response_updates_inventory_on_inventory_line(client):
    """
    Given a bot and a server inventory response line
    When _handle_response is called with that line
    Then the bot's inventory reflects the parsed values
    """
    bot = Bot(10, 20, 1, client)
    bot._handle_response("[food 10, linemate 2, deraumere 0, sibur 0, mendiane 0, phiras 0, thystame 0]")
    assert bot.inventory["food"] == 10
    assert bot.inventory["linemate"] == 2


def test_bot_handle_response_ignores_non_inventory_line(client):
    """
    Given a bot and a non-inventory server response
    When _handle_response is called with that line
    Then the bot's inventory remains all zeros
    """
    from ia.shared.enum import RESOURCES
    bot = Bot(10, 20, 1, client)
    bot._handle_response("ok")
    assert bot.inventory == {resource: 0 for resource in RESOURCES}


def test_bot_run_updates_inventory_from_server_line(client):
    """
    Given a client returning an inventory line then closing
    When the bot runs its loop
    Then the bot's inventory is updated to reflect the server response
    """
    client._responses = ["[food 7, linemate 0, deraumere 0, sibur 0, mendiane 0, phiras 0, thystame 0]"]
    bot = Bot(10, 20, 1, client)
    bot.run()
    assert bot.inventory["food"] == 7
