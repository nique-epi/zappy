"""
Unit tests for the Bot class.
"""
from unittest.mock import MagicMock
import pytest
from ia.core.bot import Bot

# pylint: disable=redefined-outer-name
# pylint: disable=protected-access

@pytest.fixture
def mock_client():
    """Fixture for mocked ZappyClient."""
    return MagicMock()

def test_bot_init(mock_client):
    """Test Bot initialization and initial state."""
    bot = Bot(10, 20, 1, mock_client)
    assert bot.width == 10
    assert bot.height == 20
    assert bot.client_num == 1
    assert bot.level == 1
    assert bot.role == "generic"
    assert len(bot.map) == 20  # height
    assert len(bot.map[0]) == 10  # width

def test_bot_assign_role(mock_client):
    """Test role assignment logic."""
    bot = Bot(10, 20, 1, mock_client)
    assert bot._assign_role(1) == "generic"

def test_bot_run_exit_on_none(mock_client):
    """Test that the main loop exits when recv returns None."""
    bot = Bot(10, 20, 1, mock_client)
    mock_client.recv.return_value = None

    # Should exit loop when recv returns None
    bot.run()
    mock_client.recv.assert_called_once()
