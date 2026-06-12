"""Unit tests for the Bot class."""
# pylint: disable=redefined-outer-name
from ia.network.exceptions import PlayerDeadError
from ia.shared.enum import Resource
from tests.IA.mocks.fake_socket import FakeSocket
from ia.core.bot import Bot


def test_bot_init(bot):
    """
    Given a world size, a connection slot and a client
    When a Bot is created
    Then its dimensions, level, role and empty map are initialised
    """
    assert bot.width == 10
    assert bot.height == 10
    assert bot.client_num == 1
    assert bot.level == 1
    assert bot.role == "generic"
    assert len(bot.map) == 10
    assert len(bot.map[0]) == 10


def test_bot_assign_role(bot):
    """
    Given a bot built from a connection slot
    When the role is assigned
    Then it defaults to the generic role
    """
    assert bot._assign_role(1) == "generic"  # pylint: disable=protected-access


def test_bot_run_exit_on_none(bot, fake_socket):
    """
    Given a client whose recv immediately returns None
    When the bot runs its loop
    Then it stops without error
    """
    fake_socket._responses = [b""]
    bot.run()
    assert not fake_socket._responses


def test_bot_run_processes_lines_then_exits(bot, fake_socket):
    """
    Given a client returning a couple of lines then a closed connection
    When the bot runs its loop
    Then all lines are consumed before stopping
    """
    fake_socket._responses = [b"ok\nko\n", b""]
    bot.run()
    assert not fake_socket._responses


def test_bot_run_exit_on_dead(client):
    """
    Given a client whose recv raises PlayerDeadError
    When the bot runs its loop
    Then it exits cleanly without propagating the error
    """
    fake_socket = FakeSocket([b"dead\n"])
    client._sock = fake_socket
    dead_bot = Bot(10, 10, 1, client)
    dead_bot.run()


def test_bot_inventory_initialised_to_zeros(bot):
    """
    Given a freshly created Bot
    When its inventory attribute is inspected
    Then every resource is set to 0
    """
    assert bot.inventory == dict.fromkeys(Resource, 0)


def test_bot_handle_response_updates_inventory_on_inventory_line(bot):
    """
    Given a bot and a server inventory response line
    When _handle_response is called with that line
    Then the bot's inventory reflects the parsed values
    """
    inventory_line = (
        "[food 10, linemate 2, deraumere 0, sibur 0, mendiane 0, phiras 0, thystame 0]"
    )
    bot._handle_response(inventory_line)  # pylint: disable=protected-access
    assert bot.inventory[Resource.FOOD] == 10
    assert bot.inventory[Resource.LINEMATE] == 2


def test_bot_handle_response_ignores_non_inventory_line(bot):
    """
    Given a bot and a non-inventory server response
    When _handle_response is called with that line
    Then the bot's inventory remains all zeros
    """
    bot._handle_response("ok")  # pylint: disable=protected-access
    assert bot.inventory == dict.fromkeys(Resource, 0)


def test_bot_run_updates_inventory_from_server_line(bot, fake_socket):
    """
    Given a client returning an inventory line then closing
    When the bot runs its loop
    Then the bot's inventory is updated to reflect the server response
    """
    inventory_line = (
        b"[food 7, linemate 0, deraumere 0, sibur 0, mendiane 0, phiras 0, thystame 0]\n"
    )
    fake_socket._responses = [inventory_line, b""]
    bot.run()
    assert bot.inventory[Resource.FOOD] == 7
