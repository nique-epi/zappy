"""
Unit tests for the ZappyClient class.
"""
from unittest.mock import MagicMock, patch
import pytest
from ia.network.client import ZappyClient

# pylint: disable=redefined-outer-name
# pylint: disable=protected-access

@pytest.fixture
def client():
    """Fixture for ZappyClient instance."""
    return ZappyClient("localhost", 4242)

def test_client_init(client):
    """Test client initialization."""
    assert client._host == "localhost"
    assert client._port == 4242
    assert client._sock is None
    assert client._recv_buf == ""

@patch("socket.socket")
def test_client_connect_success(mock_socket_class, client):
    """Test successful handshake."""
    mock_sock = MagicMock()
    mock_socket_class.return_value = mock_sock

    # Mock sequence of server responses: WELCOME, client_num, dimensions
    mock_sock.recv.side_effect = [
        b"WELCOME\n",
        b"10\n",
        b"20 30\n"
    ]

    client_num, width, height = client.connect("team1")

    assert client_num == 10
    assert width == 20
    assert height == 30
    mock_sock.connect.assert_called_with(("localhost", 4242))
    mock_sock.sendall.assert_called_with(b"team1\n")

@patch("socket.socket")
def test_client_connect_fail_welcome(mock_socket_class, client):
    """Test handshake failure on WELCOME message."""
    mock_sock = MagicMock()
    mock_socket_class.return_value = mock_sock
    mock_sock.recv.return_value = b"NOT_WELCOME\n"

    with pytest.raises(ConnectionError, match="Expected WELCOME"):
        client.connect("team1")

@patch("socket.socket")
def test_client_send(mock_socket_class, client):
    """Test sending a command."""
    _ = mock_socket_class
    mock_sock = MagicMock()
    client._sock = mock_sock

    client.send("Forward")
    mock_sock.sendall.assert_called_with(b"Forward\n")

@patch("socket.socket")
def test_client_recv(mock_socket_class, client):
    """Test receiving a line."""
    _ = mock_socket_class
    mock_sock = MagicMock()
    client._sock = mock_sock
    mock_sock.recv.return_value = b"ok\n"

    line = client.recv()
    assert line == "ok"
