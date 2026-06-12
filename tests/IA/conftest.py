"""Shared pytest fixtures for IA tests."""
# pylint: disable=redefined-outer-name,import-error
import pytest
from tests.IA.mocks.fake_client import FakeClient
from tests.IA.mocks.fake_bot import FakeBot
from tests.IA.mocks.fake_socket import FakeSocket


@pytest.fixture
def fake_client():
    """Provide a fresh FakeClient for each test."""
    return FakeClient()


@pytest.fixture
def fake_bot():
    """Provide a fresh FakeBot in SURVIVAL state for each test."""
    return FakeBot()


@pytest.fixture
def fake_socket():
    """Provide a fresh FakeSocket for each test."""
    return FakeSocket()
