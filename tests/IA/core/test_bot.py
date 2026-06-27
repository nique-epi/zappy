"""Unit tests for the Bot class."""
# pylint: disable=redefined-outer-name
from ia.core.bot import Bot
from ia.shared.enum import Resource, Role, State


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
    assert bot.role == Role.GENERIC
    assert len(bot.world_map.world_map) == 10
    assert len(bot.world_map.world_map[0]) == 10


def test_bot_role_generic_without_flag(bot):
    """
    Given a bot built without the roles bonus flag
    When its role is read
    Then it defaults to the generic role, keeping the base behaviour
    """
    assert bot.role == Role.GENERIC


def test_bot_role_assigned_when_roles_enabled(connected_client):
    """
    Given a bot built from connection slot 1 with the roles bonus enabled
    When its role is read
    Then assign_role maps the connection slot to the collector role
    """
    bot = Bot(10, 10, 1, connected_client, roles=True)
    assert bot.role == Role.COLLECTOR


def test_bot_inventory_initialised_to_zeros(bot):
    """
    Given a freshly created Bot
    When its inventory attribute is inspected
    Then every resource is set to 0
    """
    assert bot.inventory == dict.fromkeys(Resource, 0)


def test_bot_default_state_is_survival(bot):
    """
    Given a freshly created Bot
    When its state attribute is inspected
    Then it defaults to State.SURVIVAL
    """
    assert bot.state == State.SURVIVAL


def test_bot_is_incantation_chef_defaults_true(bot):
    """
    Given a freshly created Bot
    When its is_incantation_chef flag is inspected
    Then it defaults to True
    """
    assert bot.is_incantation_chef is True


def test_bot_collect_target_defaults_zero(bot):
    """
    Given a freshly created Bot
    When its collect_target attribute is inspected
    Then it defaults to 0
    """
    assert bot.collect_target == 0


def test_bot_fork_count_defaults_zero(bot):
    """
    Given a freshly created Bot
    When its fork_count attribute is inspected
    Then it defaults to 0
    """
    assert bot.fork_count == 0
