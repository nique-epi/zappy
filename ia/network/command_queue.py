"""Outgoing command queue with a bounded in-flight counter."""
from collections import deque

MAX_PENDING = 10


class CommandQueue:
    """Buffers outgoing commands and caps the number sent without a reply.

    The Zappy protocol forbids a client from having more than
    :data:`MAX_PENDING` commands awaiting a response. Commands are enqueued by
    the client and released by :meth:`pop_sendable` only while a slot is free;
    each server response frees one slot through :meth:`on_response`.
    """

    def __init__(self) -> None:
        self._outgoing: deque[str] = deque()
        self._pending = 0

    def enqueue(self, command: str) -> None:
        """Append a command to the outgoing queue.

        Args:
            command: Raw command line, without trailing newline.
        """
        self._outgoing.append(command)

    def pop_sendable(self) -> str | None:
        """Release the next command if a pending slot is available.

        Returns:
            The next command to transmit (incrementing the pending counter),
            or None when the queue is empty or the pending limit is reached.
        """
        if self._pending >= MAX_PENDING or not self._outgoing:
            return None
        self._pending += 1
        return self._outgoing.popleft()

    def on_response(self) -> None:
        """Acknowledge one server response, freeing a pending slot."""
        self._pending = max(0, self._pending - 1)

    @property
    def pending(self) -> int:
        """Number of commands sent without a response yet."""
        return self._pending

    def __len__(self) -> int:
        """Number of commands still waiting to be transmitted."""
        return len(self._outgoing)
