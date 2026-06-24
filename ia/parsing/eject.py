"""Parsing of the unsolicited ``eject: K`` notification."""


def parse_eject(line: str) -> int | None:
    """Return the direction K of an ``eject: K`` notification, or None."""
    if not line.startswith("eject:"):
        return None
    try:
        return int(line[len("eject:"):].strip())
    except ValueError:
        return None
