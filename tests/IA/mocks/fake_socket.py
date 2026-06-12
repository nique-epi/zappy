"""Fake socket covering all unit-test scenarios."""


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
