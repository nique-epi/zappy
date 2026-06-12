"""Fake Bot covering all unit-test scenarios."""
# pylint: disable=too-few-public-methods
from ia.shared.enum import Resource, State
from tests.IA.mocks.fake_client import FakeClient


class FakeBot:
    """Minimal Bot stand-in for unit tests."""

    def __init__(self, width=10, height=10, client_num=1, client=None):
        self.width = width
        self.height = height
        self.client_num = client_num
        self.client = client if client is not None else FakeClient()
        self.inventory = dict.fromkeys(Resource, 0)
        self.state = State.SURVIVAL
        self.ran = False

    def run(self):
        self.ran = True
