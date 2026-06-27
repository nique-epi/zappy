#!/usr/bin/env python3
"""Main entry point for the Zappy AI client."""
import argparse
import sys
import threading
from ia.network.client import ZappyClient
from ia.core.bot import Bot
from ia.core.player_spawner import PlayerSpawner, run_player


def parse_arguments(argv=None) -> argparse.Namespace:
    """Parse command line arguments.

    Returns:
        argparse.Namespace: Parsed arguments.
    """
    parser = argparse.ArgumentParser(
        prog="zappy_ai", usage="./zappy_ai -p port -n name -h machine",
        add_help=False)
    parser.add_argument("--help", action="help",
                        default=argparse.SUPPRESS,
                        help="show this help message and exit")
    parser.add_argument("-p", type=int, required=True,
                        dest="port",    help="port number")
    parser.add_argument("-n", type=str, required=True,
                        dest="name",    help="team name")
    parser.add_argument("-h", type=str, default="localhost",
                        dest="host", help="hostname")
    parser.add_argument("-b", action="store_true",
                        dest="mental_map",
                        help="enable the mental-map pathfinding bonus")
    parser.add_argument("-r", "--roles", action="store_true",
                        dest="roles",
                        help="enable the fixed role specialisation bonus")

    args = parser.parse_args(argv)
    if not 1 <= args.port <= 65535:
        parser.error(f"Invalid port: {args.port}")
    if not args.name.strip():
        parser.error("Team name cannot be empty")

    return args


def main(
    argv=None,
    client_factory=ZappyClient,
    bot_factory=Bot,
    thread_factory=threading.Thread,
) -> None:
    """Main entry point."""
    args = parse_arguments(argv)
    spawner = PlayerSpawner(args, client_factory, bot_factory, thread_factory)

    client = client_factory(args.host, args.port)

    try:
        client_num, width, height = client.connect(args.name)
    except ConnectionError as e:
        print(f"[ERROR] {e}", file=sys.stderr)
        sys.exit(1)

    run_player(width, height, client_num, client, spawner, depth=0)
    spawner.join_all()


if __name__ == "__main__":
    main()
