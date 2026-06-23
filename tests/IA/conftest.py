"""Shared pytest fixtures for IA tests."""
# pylint: disable=redefined-outer-name,import-error
import pytest
from ia.network.client import ZappyClient
from ia.core.bot import Bot
from tests.IA.mocks.fake_socket import FakeSocket


@pytest.fixture
def fake_socket():
    """Provide a fresh FakeSocket for each test."""
    return FakeSocket()


@pytest.fixture
def client(fake_socket):
    """Provide a ZappyClient not yet connected, for testing the handshake."""
    return ZappyClient("localhost", 4242, sock_factory=lambda: fake_socket)


@pytest.fixture
def connected_client(fake_socket):
    """Provide a ZappyClient already connected via FakeSocket."""
    zc = ZappyClient("localhost", 4242, sock_factory=lambda: fake_socket)
    zc._sock = fake_socket
    return zc


@pytest.fixture
def bot(connected_client):
    """Provide a real Bot wired to the test client."""
    return Bot(10, 10, 1, connected_client)
