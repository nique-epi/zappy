"""Client for the Zappy game server protocol."""
import socket
from typing import Callable, Final
from ia.network.buffer import Buffer
from ia.network.command_queue import CommandQueue
from ia.network.exceptions import PlayerDeadError

BUFFER_SIZE: Final = 4096
DEAD_MESSAGE: Final = "dead"


class ZappyClient:
    """Network client for communicating with the Zappy server."""

    def __init__(
        self,
        host: str,
        port: int,
        sock_factory: Callable[[], socket.socket] = lambda: socket.socket(
            socket.AF_INET, socket.SOCK_STREAM
        ),
    ) -> None:
        self._host = host
        self._port = port
        self._sock: socket.socket | None = None
        self._sock_factory = sock_factory
        self._buffer = Buffer()
        self._queue = CommandQueue()
        self._eof = False
        self._notifications: list[str] = []

    @property
    def connected(self) -> bool:
        """True while a socket is open and no EOF has been read yet."""
        return self._sock is not None and not self._eof

    def connect(self, team_name: str) -> tuple[int, int, int]:
        """Performs the complete handshake.
        Returns (client_num, width, height).
        Raises ConnectionError if the server responds incorrectly."""
        self._sock = self._sock_factory()
        try:
            self._sock.connect((self._host, self._port))
        except OSError as e:
            raise ConnectionError(
                f"Cannot connect to {self._host}:{self._port} — {e}") from e

        welcome = self._recv_line()
        if welcome != "WELCOME":
            raise ConnectionError(f"Expected WELCOME, got: {welcome!r}")

        self._send_line(team_name)

        client_num_line = self._recv_line()
        if client_num_line is None:
            raise ConnectionError("Server closed connection after team name")
        try:
            client_num = int(client_num_line)
        except ValueError as e:
            raise ConnectionError(
                f"Server refused connection: {client_num_line!r}") from e

        world_line = self._recv_line()
        try:
            width, height = map(int, world_line.split())
        except (ValueError, AttributeError) as e:
            raise ConnectionError(
                f"Invalid world dimensions: {world_line!r}") from e

        return client_num, width, height

    def _send_line(self, msg: str) -> None:
        """Sends a line terminated by \\n, treating a dead socket as EOF."""
        try:
            self._sock.sendall((msg + "\n").encode())
        except OSError:
            self._eof = True

    def _fill_buffer(self) -> bool:
        """Reads one chunk from the socket into the reception buffer.

        Returns:
            True if data was read, False if the server closed the connection.
        """
        try:
            chunk = self._sock.recv(BUFFER_SIZE).decode(errors="replace")
        except OSError:
            self._eof = True
            return False
        if not chunk:
            self._eof = True
            return False
        self._buffer.feed(chunk)
        return True

    def _recv_line(self) -> str | None:
        """Reads a complete line, blocking on the socket only when needed.

        Returns:
            The next complete line, or None if the connection was closed.
        """
        while not self._buffer.has_line():
            if not self._fill_buffer():
                return None
        return self._buffer.pop_line()

    def _recv_line_timeout(self, timeout: float) -> str | None:
        """Reads a complete line, or None on timeout or closed connection."""
        while not self._buffer.has_line():
            self._sock.settimeout(timeout)
            try:
                chunk = self._sock.recv(BUFFER_SIZE).decode(errors="replace")
            except TimeoutError:
                return None
            except OSError:
                self._eof = True
                return None
            finally:
                self._sock.settimeout(None)
            if not chunk:
                self._eof = True
                return None
            self._buffer.feed(chunk)
        return self._buffer.pop_line()

    def _flush(self) -> None:
        """Transmits queued commands while a pending slot is available."""
        while (command := self._queue.pop_sendable()) is not None:
            self._send_line(command)

    def send(self, command: str) -> None:
        """Queues a command and transmits it while below the pending limit."""
        self._queue.enqueue(command)
        self._flush()

    def recv(self) -> str | None:
        """Reads one server response, freeing a pending command slot.

        Returns:
            The next command response line, or None if the connection closed.
        Raises:
            PlayerDeadError: When the server reports the player is dead.
        """
        line = self._recv_line()
        if line is None:
            return None
        if line == DEAD_MESSAGE:
            raise PlayerDeadError()
        if line.startswith("message ") or line.startswith("eject:"):
            self._notifications.append(line)
            return line
        self._queue.on_response()
        self._flush()
        return line

    def recv_timeout(self, timeout: float) -> str | None:
        """Reads one response, or None on timeout/closed; raises on death."""
        line = self._recv_line_timeout(timeout)
        if line is None:
            return None
        if line == DEAD_MESSAGE:
            raise PlayerDeadError()
        if line.startswith("message ") or line.startswith("eject:"):
            self._notifications.append(line)
            return line
        self._queue.on_response()
        self._flush()
        return line

    def recv_ack(self) -> str | None:
        """Reads the next ack, queuing any broadcast/eject heard first."""
        while True:
            line = self.recv()
            if line is None:
                return None
            if line.startswith("message ") or line.startswith("eject:"):
                continue
            return line

    def pop_notification(self) -> str | None:
        """Pops the oldest notification queued by a prior recv_ack() call."""
        if not self._notifications:
            return None
        return self._notifications.pop(0)

    def close(self) -> None:
        """Closes the socket and resets the buffering state."""
        if self._sock:
            self._sock.close()
            self._sock = None
            self._buffer = Buffer()
            self._queue = CommandQueue()
            self._notifications = []
