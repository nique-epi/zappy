"""Unit tests for the Bot class."""
# pylint: disable=redefined-outer-name
from ia.shared.enum import Resource, State


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
    assert len(bot.world_map.world_map) == 10
    assert len(bot.world_map.world_map[0]) == 10


def test_bot_assign_role(bot):
    """
    Given a bot built from a connection slot
    When the role is assigned
    Then it defaults to the generic role
    """
    assert bot._assign_role(1) == "generic"  # pylint: disable=protected-access


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
