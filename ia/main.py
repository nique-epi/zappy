"""Main entry point for the Zappy AI client."""
import argparse
import sys
from .network.client import ZappyClient
from .core.bot import Bot


def parse_arguments() -> argparse.Namespace:
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

    args = parser.parse_args()
    if not 1 <= args.port <= 65535:
        parser.error(f"Invalid port: {args.port}")
    if not args.name.strip():
        parser.error("Team name cannot be empty")

    return args


def main() -> None:
    """Main entry point."""
    args = parse_arguments()

    client = ZappyClient(args.host, args.port)

    try:
        client_num, width, height = client.connect(args.name)
    except ConnectionError as e:
        print(f"[ERROR] {e}", file=sys.stderr)
        sys.exit(1)

    bot = Bot(width, height, client_num, client)
    bot.run()


if __name__ == "__main__":
    main()
