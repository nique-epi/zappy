"""Line-oriented reception buffer for the Zappy protocol."""


class Buffer:
    """Accumulates raw socket data and yields complete lines.

    The Zappy server speaks a newline-delimited protocol over TCP. A single
    ``recv()`` may return several lines, a partial line, or both. This buffer
    stores the raw bytes already decoded to text and hands them back one
    complete line at a time through :meth:`pop_line`.
    """

    def __init__(self) -> None:
        self._data = ""

    def feed(self, data: str) -> None:
        """Append freshly received data to the buffer.

        Args:
            data: Decoded chunk read from the socket.
        """
        self._data += data

    def has_line(self) -> bool:
        """Return True if at least one complete line is buffered."""
        return "\n" in self._data

    def pop_line(self) -> str | None:
        """Remove and return the next complete line.

        Returns:
            The next line without its trailing newline, or None when no
            complete line is buffered yet.
        """
        if "\n" not in self._data:
            return None
        line, self._data = self._data.split("\n", 1)
        return line.rstrip("\r")
