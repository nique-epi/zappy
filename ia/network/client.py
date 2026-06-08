"""Client for the Zappy game server protocol."""
import socket

CONST_BUFFER_SIZE = 4096


class ZappyClient:
    """Network client for communicating with the Zappy server."""

    def __init__(self, host: str, port: int) -> None:
        self._host = host
        self._port = port
        self._sock: socket.socket | None = None
        self._recv_buf = ""

    def connect(self, team_name: str) -> tuple[int, int, int]:
        """Performs the complete handshake.
        Returns (client_num, width, height).
        Raises ConnectionError if the server refuses or responds incorrectly."""
        self._sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
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
        """Sends a line terminated by \\n."""
        self._sock.sendall((msg + "\n").encode())

    def _recv_line(self) -> str | None:
        """
        Reads a complete line from the socket.
        Buffers partial data.
        """
        while "\n" not in self._recv_buf:
            chunk = self._sock.recv(CONST_BUFFER_SIZE).decode(errors="replace")
            if not chunk:
                return None
            self._recv_buf += chunk

        line, self._recv_buf = self._recv_buf.split("\n", 1)
        return line.strip()

    def send(self, command: str) -> None:
        """Public API used by the bot to send commands."""
        self._send_line(command)

    def recv(self) -> str | None:
        """Public API: reads a response from the server."""
        return self._recv_line()

    def close(self) -> None:
        """Closes the socket."""
        if self._sock:
            self._sock.close()
            self._sock = None
