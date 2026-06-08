"""
Unit tests for the Bot class.
"""
import pytest
from ia.core.bot import Bot


class FakeClient:
    """Minimal ZappyClient stand-in for unit tests."""

    def __init__(self, responses=None):
        self._responses = list(responses or [])
        self.sent = []
        self.recv_calls = 0

    def recv(self):
        self.recv_calls += 1
        if not self._responses:
            return None
        return self._responses.pop(0)

    def send(self, command):
        self.sent.append(command)

    def close(self):
        pass


@pytest.fixture
def client():
    return FakeClient()


def test_bot_init(client):
    bot = Bot(10, 20, 1, client)
    assert bot.width == 10
    assert bot.height == 20
    assert bot.client_num == 1
    assert bot.level == 1
    assert bot.role == "generic"
    assert len(bot.map) == 20
    assert len(bot.map[0]) == 10


def test_bot_assign_role(client):
    bot = Bot(10, 20, 1, client)
    assert bot._assign_role(1) == "generic"


def test_bot_run_exit_on_none(client):
    bot = Bot(10, 20, 1, client)
    bot.run()
    assert client.recv_calls == 1
