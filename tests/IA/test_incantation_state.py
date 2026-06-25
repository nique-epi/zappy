"""Unit tests for IncantationState."""
# pylint: disable=redefined-outer-name,protected-access
from ia.config import FOOD_THRESHOLD
from ia.core.bot import Bot
from ia.game.elevation import MAX_LEVEL
from ia.network.client import ZappyClient
from ia.shared.enum import Resource, State
from ia.states.incantation import IncantationState
from tests.IA.mocks.fake_socket import FakeSocket


def _make_bot(responses: list, level: int = 1, chef: bool = True) -> Bot:
    """Build a Bot wired to a FakeSocket with preset server responses."""
    encoded = [
        r if isinstance(r, bytes)
        else (r.encode() + b"\n") if r else b""
        for r in responses
    ]
    fake_socket = FakeSocket(encoded)
    zc = ZappyClient("localhost", 4242, sock_factory=lambda: fake_socket)
    zc._sock = fake_socket
    bot = Bot(10, 10, 1, zc)
    bot.level = level
    bot.inventory = dict.fromkeys(Resource, 0)
    bot.inventory[Resource.FOOD] = 25
    bot.is_incantation_chef = chef
    return bot


class TestChefIncantation:
    """Tests for IncantationState when the bot is the incantation chef."""

    def test_sends_incantation_command(self):
        """
        Given a chef bot with a successful server response
        When handle is called
        Then Incantation is sent to the server
        """
        bot = _make_bot(["elevation underway", "Current level: 2"])
        state = IncantationState(bot)
        state.handle()
        sent = b"".join(bot.client._sock.sent).decode()
        assert "Incantation" in sent

    def test_returns_survival_on_success(self):
        """
        Given a chef bot receiving Current level: 2
        When handle is called
        Then State.SURVIVAL is returned
        """
        bot = _make_bot(["elevation underway", "Current level: 2"])
        assert IncantationState(bot).handle() == State.SURVIVAL

    def test_updates_bot_level_on_success(self):
        """
        Given a chef bot at level 1 receiving Current level: 2
        When handle is called
        Then bot.level is updated to 2
        """
        bot = _make_bot(["elevation underway", "Current level: 2"])
        IncantationState(bot).handle()
        assert bot.level == 2

    def test_returns_survival_on_ko(self):
        """
        Given a chef bot and a server responding ko to Incantation
        When handle is called
        Then State.SURVIVAL is returned so the bot resumes collecting
        """
        bot = _make_bot(["ko"])
        assert IncantationState(bot).handle() == State.SURVIVAL

    def test_does_not_update_level_on_ko(self):
        """
        Given a chef bot at level 1 and a server responding ko
        When handle is called
        Then bot.level stays at 1
        """
        bot = _make_bot(["ko"])
        IncantationState(bot).handle()
        assert bot.level == 1

    def test_returns_survival_on_disconnect(self):
        """
        Given a chef bot whose connection drops during incantation
        When handle is called
        Then State.SURVIVAL is returned
        """
        bot = _make_bot([])
        assert IncantationState(bot).handle() == State.SURVIVAL

    def test_returns_survival_on_timeout(self):
        """
        Given a chef bot receiving only noise from the server
        When handle times out without a conclusive response
        Then State.SURVIVAL is returned
        """
        bot = _make_bot(["noise"] * 100)
        assert IncantationState(bot).handle() == State.SURVIVAL

    def test_never_sends_inventory_while_waiting_for_result(self):
        """
        Given a chef bot waiting for the ritual's result among noise lines
        When handle times out without a conclusive response
        Then no Inventory command is sent while the player is frozen

        Sending one would have its reply read as the answer to whatever
        command is next in the queue, desynchronizing the whole stream.
        """
        bot = _make_bot(["noise"] * 100)
        IncantationState(bot).handle()
        sent = b"".join(bot.client._sock.sent).decode()
        assert "Inventory" not in sent

    def test_ignores_noise_before_current_level(self):
        """
        Given a chef bot receiving noise then Current level: 3
        When handle is called
        Then bot.level is updated to 3 and State.SURVIVAL is returned
        """
        bot = _make_bot(
            ["elevation underway", "some noise", "Current level: 3"],
            level=2,
        )
        result = IncantationState(bot).handle()
        assert result == State.SURVIVAL
        assert bot.level == 3

    def test_waits_for_food_before_incantating(self):
        """
        Given a chef bot whose food is at or below the critical threshold
        When handle is called
        Then it returns EATING without sending Incantation
        """
        bot = _make_bot([])
        bot.inventory[Resource.FOOD] = FOOD_THRESHOLD
        result = IncantationState(bot).handle()
        sent = b"".join(bot.client._sock.sent).decode()
        assert result == State.EATING
        assert "Incantation" not in sent

    def test_reaches_max_level_without_exiting(self):
        """
        Given a chef bot at level 7 receiving Current level: 8
        When handle is called
        Then bot.level is updated to 8 and State.SURVIVAL is returned
        """
        bot = _make_bot(["Current level: 8"], level=MAX_LEVEL - 1)
        result = IncantationState(bot).handle()
        assert result == State.SURVIVAL
        assert bot.level == MAX_LEVEL


class TestFollowerIncantation:
    """Tests for IncantationState when the bot is an incantation follower."""

    def test_does_not_send_incantation_command(self):
        """
        Given a follower bot with a successful server response
        When handle is called
        Then no Incantation command is sent
        """
        bot = _make_bot(["Current level: 2"], chef=False)
        IncantationState(bot).handle()
        sent = b"".join(bot.client._sock.sent).decode()
        assert "Incantation" not in sent

    def test_returns_survival_on_success(self):
        """
        Given a follower bot receiving Current level: 2
        When handle is called
        Then State.SURVIVAL is returned
        """
        bot = _make_bot(["Current level: 2"], chef=False)
        assert IncantationState(bot).handle() == State.SURVIVAL

    def test_updates_bot_level_on_success(self):
        """
        Given a follower bot at level 1 receiving Current level: 2
        When handle is called
        Then bot.level is updated to 2
        """
        bot = _make_bot(["Current level: 2"], chef=False)
        IncantationState(bot).handle()
        assert bot.level == 2

    def test_returns_survival_on_ko(self):
        """
        Given a follower bot and a server responding ko
        When handle is called
        Then State.SURVIVAL is returned
        """
        bot = _make_bot(["ko"], chef=False)
        assert IncantationState(bot).handle() == State.SURVIVAL

    def test_reaches_max_level_without_exiting(self):
        """
        Given a follower bot at level 7 receiving Current level: 8
        When handle is called
        Then bot.level is updated to 8 and State.SURVIVAL is returned
        """
        bot = _make_bot(["Current level: 8"], level=MAX_LEVEL - 1, chef=False)
        result = IncantationState(bot).handle()
        assert result == State.SURVIVAL
        assert bot.level == MAX_LEVEL
