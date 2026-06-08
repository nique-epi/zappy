"""
Unit tests for the ZappyClient class.
"""
import pytest
from ia.network.client import ZappyClient
from ia.network.exceptions import PlayerDeadError


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
    """
    Given a freshly constructed client
    When inspecting its initial state
    Then host/port are stored, no socket is open and the buffers are empty
    """
    zc = ZappyClient("localhost", 4242, sock_factory=lambda: fake_socket)
    assert zc._host == "localhost"
    assert zc._port == 4242
    assert zc._sock is None
    assert zc._buffer.has_line() is False
    assert zc._queue.pending == 0
    assert len(zc._queue) == 0


def test_client_connect_success(client, fake_socket):
    """
    Given a server replying WELCOME, a client number and world dimensions
    When the handshake runs
    Then the client number and map size are returned and the team name is sent
    """
    fake_socket._responses = [b"WELCOME\n", b"10\n", b"20 30\n"]

    client_num, width, height = client.connect("team1")

    assert client_num == 10
    assert width == 20
    assert height == 30
    assert fake_socket.connected_to == ("localhost", 4242)
    assert b"team1\n" in fake_socket.sent


def test_client_connect_os_error(client, fake_socket):
    """
    Given a socket that fails to connect
    When connect is called
    Then a ConnectionError describing the failure is raised
    """
    fake_socket.connect_error = OSError("Conn refused")

    with pytest.raises(ConnectionError, match="Cannot connect"):
        client.connect("team1")


def test_client_connect_fail_welcome(client, fake_socket):
    """
    Given a server whose first message is not WELCOME
    When the handshake runs
    Then a ConnectionError about the missing WELCOME is raised
    """
    fake_socket._responses = [b"NOT_WELCOME\n"]

    with pytest.raises(ConnectionError, match="Expected WELCOME"):
        client.connect("team1")


def test_client_connect_closed_after_team(client, fake_socket):
    """
    Given a server that closes the connection after the team name
    When the handshake runs
    Then a ConnectionError about the closed connection is raised
    """
    fake_socket._responses = [b"WELCOME\n", b""]

    with pytest.raises(ConnectionError, match="Server closed connection"):
        client.connect("team1")


def test_client_connect_refused_num(client, fake_socket):
    """
    Given a server returning a non-integer client number
    When the handshake runs
    Then a ConnectionError about the refused connection is raised
    """
    fake_socket._responses = [b"WELCOME\n", b"ko\n"]

    with pytest.raises(ConnectionError, match="Server refused connection"):
        client.connect("team1")


def test_client_connect_invalid_dims(client, fake_socket):
    """
    Given a server returning malformed world dimensions
    When the handshake runs
    Then a ConnectionError about invalid dimensions is raised
    """
    fake_socket._responses = [b"WELCOME\n", b"10\n", b"20\n"]

    with pytest.raises(ConnectionError, match="Invalid world dimensions"):
        client.connect("team1")


def test_client_send(client, fake_socket):
    """
    Given a connected client
    When a command is sent
    Then the command is transmitted with a trailing newline
    """
    client._sock = fake_socket

    client.send("Forward")

    assert b"Forward\n" in fake_socket.sent


def test_client_recv(client, fake_socket):
    """
    Given a socket holding one complete line
    When recv is called
    Then the line is returned without its newline
    """
    fake_socket._responses = [b"ok\n"]
    client._sock = fake_socket

    assert client.recv() == "ok"


def test_client_recv_none(client, fake_socket):
    """
    Given a socket reporting a closed connection
    When recv is called
    Then None is returned
    """
    fake_socket._responses = [b""]
    client._sock = fake_socket

    assert client.recv() is None


def test_client_close(client, fake_socket):
    """
    Given a client owning an open socket
    When close is called
    Then the socket is closed and cleared
    """
    client._sock = fake_socket

    client.close()

    assert fake_socket.closed
    assert client._sock is None


def test_client_close_none(client):
    """
    Given a client with no socket
    When close is called
    Then it is a no-op and leaves the socket unset
    """
    client._sock = None
    client.close()
    assert client._sock is None


def test_client_recv_multiple_lines_single_recv(client, fake_socket):
    """
    Given a socket delivering several lines in a single recv
    When recv is called repeatedly
    Then each buffered line is returned in order
    """
    fake_socket._responses = [b"ok\nko\nmehari\n"]
    client._sock = fake_socket

    assert client.recv() == "ok"
    assert client.recv() == "ko"
    assert client.recv() == "mehari"


def test_client_send_blocks_at_pending_limit(client, fake_socket):
    """
    Given more commands sent than the pending limit allows
    When a server response later frees a slot
    Then only ten are transmitted at first and the next is flushed on response
    """
    client._sock = fake_socket

    for i in range(12):
        client.send(f"cmd{i}")

    assert len(fake_socket.sent) == 10
    assert client._queue.pending == 10
    assert len(client._queue) == 2

    fake_socket._responses = [b"ok\n"]
    assert client.recv() == "ok"

    assert len(fake_socket.sent) == 11
    assert client._queue.pending == 10
    assert len(client._queue) == 1


def test_client_recv_dead_raises(client, fake_socket):
    """
    Given a server sending the "dead" message
    When recv is called
    Then PlayerDeadError is raised
    """
    fake_socket._responses = [b"dead\n"]
    client._sock = fake_socket

    with pytest.raises(PlayerDeadError):
        client.recv()
