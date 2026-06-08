"""
Unit tests for the ZappyClient class.
"""
import pytest
from ia.network.client import ZappyClient


class FakeSocket:
    """Minimal socket stand-in for unit tests."""

    def __init__(self, responses=None):
        self._responses = list(responses or [])
        self.connected_to = None
        self.sent = []
        self.closed = False
        self.connect_error = None

    def connect(self, address):
        if self.connect_error:
            raise self.connect_error
        self.connected_to = address

    def sendall(self, data):
        self.sent.append(data)

    def recv(self, _bufsize):
        if not self._responses:
            return b""
        return self._responses.pop(0)

    def close(self):
        self.closed = True


@pytest.fixture
def fake_socket():
    return FakeSocket()


@pytest.fixture
def client(fake_socket):
    return ZappyClient("localhost", 4242, sock_factory=lambda: fake_socket)


def test_client_init(fake_socket):
    zc = ZappyClient("localhost", 4242, sock_factory=lambda: fake_socket)
    assert zc._host == "localhost"
    assert zc._port == 4242
    assert zc._sock is None
    assert zc._recv_buf == ""


def test_client_connect_success(client, fake_socket):
    fake_socket._responses = [b"WELCOME\n", b"10\n", b"20 30\n"]

    client_num, width, height = client.connect("team1")

    assert client_num == 10
    assert width == 20
    assert height == 30
    assert fake_socket.connected_to == ("localhost", 4242)
    assert b"team1\n" in fake_socket.sent


def test_client_connect_os_error(client, fake_socket):
    fake_socket.connect_error = OSError("Conn refused")

    with pytest.raises(ConnectionError, match="Cannot connect"):
        client.connect("team1")


def test_client_connect_fail_welcome(client, fake_socket):
    fake_socket._responses = [b"NOT_WELCOME\n"]

    with pytest.raises(ConnectionError, match="Expected WELCOME"):
        client.connect("team1")


def test_client_connect_closed_after_team(client, fake_socket):
    fake_socket._responses = [b"WELCOME\n", b""]

    with pytest.raises(ConnectionError, match="Server closed connection"):
        client.connect("team1")


def test_client_connect_refused_num(client, fake_socket):
    fake_socket._responses = [b"WELCOME\n", b"ko\n"]

    with pytest.raises(ConnectionError, match="Server refused connection"):
        client.connect("team1")


def test_client_connect_invalid_dims(client, fake_socket):
    fake_socket._responses = [b"WELCOME\n", b"10\n", b"20\n"]

    with pytest.raises(ConnectionError, match="Invalid world dimensions"):
        client.connect("team1")


def test_client_send(client, fake_socket):
    client._sock = fake_socket

    client.send("Forward")

    assert b"Forward\n" in fake_socket.sent


def test_client_recv(client, fake_socket):
    fake_socket._responses = [b"ok\n"]
    client._sock = fake_socket

    assert client.recv() == "ok"


def test_client_recv_none(client, fake_socket):
    fake_socket._responses = [b""]
    client._sock = fake_socket

    assert client.recv() is None


def test_client_close(client, fake_socket):
    client._sock = fake_socket

    client.close()

    assert fake_socket.closed
    assert client._sock is None


def test_client_close_none(client):
    client._sock = None
    client.close()
    assert client._sock is None
