"""Unit tests for PlayerSpawner."""
from ia.core.player_spawner import PlayerSpawner


class _NoOpThread:
    """A fake thread that records construction but never runs."""

    def __init__(self, target, args=()):
        self.target = target
        self.args = args
        self.started = False

    def start(self):
        self.started = True

    def is_alive(self):
        return False

    def join(self, timeout=None):
        pass


def _make_spawner() -> PlayerSpawner:
    """Build a PlayerSpawner with factories that are never called."""
    return PlayerSpawner(
        args=None, client_factory=None, bot_factory=None,
        thread_factory=_NoOpThread,
    )


def test_spawn_player_starts_a_thread():
    """
    Given a fresh spawner
    When spawn_player is called once
    Then the new thread is registered and started
    """
    spawner = _make_spawner()
    spawner.spawn_player(depth=1)
    assert len(spawner.threads) == 1
    assert spawner.threads[0].started is True


def test_spawn_player_passes_depth_to_connect_and_run():
    """
    Given a fresh spawner
    When spawn_player is called with a given depth
    Then the spawned thread's target receives that depth as an argument
    """
    spawner = _make_spawner()
    spawner.spawn_player(depth=2)
    assert spawner.threads[0].args[-1] == 2
