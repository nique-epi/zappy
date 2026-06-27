"""Spawns additional in-process players when a bot forks."""
import argparse
import sys
import threading
from ia.core.fsm import StateMachine


class PlayerSpawner:
    """Spawns one in-process player per fork, on its own thread."""

    def __init__(
        self, args: argparse.Namespace,
        client_factory, bot_factory, thread_factory,
    ) -> None:
        self.args = args
        self.client_factory = client_factory
        self.bot_factory = bot_factory
        self.thread_factory = thread_factory
        self.threads: list[threading.Thread] = []
        self.lock = threading.Lock()

    def register_thread(self, thread: threading.Thread) -> None:
        """Track a spawned player thread so join_all can wait on it."""
        with self.lock:
            self.threads.append(thread)

    def spawn_player(self, depth: int) -> None:
        """Start one more in-process player at the given fork depth."""
        thread = self.thread_factory(
            target=connect_and_run, args=(self.args, self, depth)
        )
        self.register_thread(thread)
        thread.start()

    def join_all(self) -> None:
        """Block until every spawned player thread has finished."""
        while True:
            with self.lock:
                pending = [t for t in self.threads if t.is_alive()]
            if not pending:
                break
            for thread in pending:
                thread.join(timeout=0.5)


# pylint: disable-next=too-many-arguments,too-many-positional-arguments
def run_player(
    width: int,
    height: int,
    client_num: int,
    client,
    spawner: PlayerSpawner,
    depth: int,
) -> None:
    """Build the bot for an already-connected client, then run it."""
    bot = spawner.bot_factory(
        width, height, client_num, client,
        mental_map=spawner.args.mental_map,
        roles=spawner.args.roles,
    )
    bot.fork_depth = depth
    bot.spawn_player = spawner.spawn_player
    try:
        StateMachine(bot).run()
    finally:
        client.close()


def connect_and_run(
    args: argparse.Namespace, spawner: PlayerSpawner, depth: int
) -> None:
    """Connect one player and drive it until death/disconnect."""
    client = spawner.client_factory(args.host, args.port)
    try:
        client_num, width, height = client.connect(args.name)
    except ConnectionError as e:
        print(f"[ERROR] {e}", file=sys.stderr)
        return

    run_player(width, height, client_num, client, spawner, depth)
