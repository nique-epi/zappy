"""Main entry point for the Zappy AI client."""
import argparse
import sys
from ia.network.client import ZappyClient
from ia.core.bot import Bot
from ia.core.fsm import StateMachine


def parse_arguments(argv=None) -> argparse.Namespace:
    """Parse command line arguments.

    Returns:
        argparse.Namespace: Parsed arguments.
    """
    parser = argparse.ArgumentParser(
        prog="zappy_ai", usage="./zappy_ai -p port -n name -h machine",
        add_help=False)
    parser.add_argument("-p", type=int, required=True,
                        dest="port",    help="port number")
    parser.add_argument("-n", type=str, required=True,
                        dest="name",    help="team name")
    parser.add_argument("-h", type=str, default="localhost",
                        dest="host", help="hostname")

    args = parser.parse_args(argv)
    if not 1 <= args.port <= 65535:
        parser.error(f"Invalid port: {args.port}")
    if not args.name.strip():
        parser.error("Team name cannot be empty")

    return args


def main(argv=None, client_factory=ZappyClient, bot_factory=Bot) -> None:
    """Main entry point."""
    args = parse_arguments(argv)

    client = client_factory(args.host, args.port)

    try:
        client_num, width, height = client.connect(args.name)
    except ConnectionError as e:
        print(f"[ERROR] {e}", file=sys.stderr)
        sys.exit(1)

    bot = bot_factory(width, height, client_num, client)
    machine = StateMachine(bot)
    try:
        machine.run()
    finally:
        client.close()


if __name__ == "__main__":
    main()
