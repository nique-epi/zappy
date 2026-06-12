"""Fake ZappyClient covering all unit-test scenarios."""


class FakeClient:
    """Minimal ZappyClient stand-in for unit tests."""

    def __init__(self, responses=None, connect_result=(1, 10, 20),
                 connect_error=None):
        self.responses = list(responses or [])
        self.sent = []
        self.recv_calls = 0
        self._connect_result = connect_result
        self._connect_error = connect_error
        self.connect_arg = None
        self.closed = False

    def connect(self, team_name):
        self.connect_arg = team_name
        if self._connect_error:
            raise self._connect_error
        return self._connect_result

    def send(self, command):
        self.sent.append(command)

    def recv(self):
        self.recv_calls += 1
        if not self.responses:
            return None
        item = self.responses.pop(0)
        if isinstance(item, Exception):
            raise item
        return item

    def close(self):
        self.closed = True
