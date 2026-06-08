"""
Unit tests for the main module.
"""
from unittest.mock import patch, MagicMock
import pytest
from ia.main import parse_arguments, main

def test_parse_arguments_success():
    """Test successful argument parsing."""
    test_args = ["prog", "-p", "4242", "-n", "team1", "-h", "localhost"]
    with patch("sys.argv", test_args):
        args = parse_arguments()
        assert args.port == 4242
        assert args.name == "team1"
        assert args.host == "localhost"

def test_parse_arguments_invalid_port():
    """Test argument parsing with invalid port."""
    test_args = ["prog", "-p", "70000", "-n", "team1"]
    with patch("sys.argv", test_args):
        with pytest.raises(SystemExit):
            parse_arguments()

def test_parse_arguments_empty_name():
    """Test argument parsing with empty team name."""
    test_args = ["prog", "-p", "4242", "-n", "  "]
    with patch("sys.argv", test_args):
        with pytest.raises(SystemExit):
            parse_arguments()

@patch("ia.main.parse_arguments")
@patch("ia.main.ZappyClient")
@patch("ia.main.Bot")
def test_main_success(mock_bot_class, mock_client_class, mock_parse_args):
    """Test the main function's successful execution path."""
    mock_args = MagicMock()
    mock_args.port = 4242
    mock_args.name = "team1"
    mock_args.host = "localhost"
    mock_parse_args.return_value = mock_args

    mock_client = MagicMock()
    mock_client.connect.return_value = (1, 10, 10)
    mock_client_class.return_value = mock_client

    mock_bot = MagicMock()
    mock_bot_class.return_value = mock_bot

    main()

    mock_client.connect.assert_called_with("team1")
    mock_bot.run.assert_called_once()

@patch("ia.main.parse_arguments")
@patch("ia.main.ZappyClient")
def test_main_connection_error(mock_client_class, mock_parse_args):
    """Test main function handling of connection errors."""
    mock_args = MagicMock()
    mock_parse_args.return_value = mock_args

    mock_client = MagicMock()
    mock_client.connect.side_effect = ConnectionError("Fail")
    mock_client_class.return_value = mock_client

    with pytest.raises(SystemExit) as exc:
        main()
    assert exc.value.code == 1
