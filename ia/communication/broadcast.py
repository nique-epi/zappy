"""Broadcast protocol for inter-bot coordination."""
from dataclasses import dataclass
from enum import Enum

PREFIX = "ZAPPY"
SEPARATOR = ":"


class MessageType(Enum):
    """Types of broadcast messages exchanged between team bots."""
    READY = "READY"
    LEAD = "LEAD"
    JOIN = "JOIN"
    FORK_NEEDED = "FORK_NEEDED"


@dataclass
class BroadcastMessage:
    """Parsed representation of a received broadcast."""
    direction: int
    msg_type: MessageType
    level: int
    data: str


def format_message(msg_type: MessageType, level: int, data: str = "") -> str:
    """Build a broadcast payload to send via the Broadcast command."""
    return SEPARATOR.join([PREFIX, msg_type.value, str(level), data])


def _parse_payload(payload: str) -> tuple[MessageType, int, str] | None:
    """Extract msg_type, level and data from a ZAPPY-prefixed payload."""
    parts = payload.split(SEPARATOR, 3)
    if len(parts) < 4 or parts[0] != PREFIX:
        return None
    try:
        msg_type = MessageType(parts[1])
        level = int(parts[2])
    except ValueError:
        return None
    return msg_type, level, parts[3]


def parse_broadcast(line: str) -> BroadcastMessage | None:
    """Parse a raw server broadcast line into a BroadcastMessage, or None."""
    if not line.startswith("message "):
        return None
    after_keyword = line[len("message "):]
    comma = after_keyword.find(",")
    if comma == -1:
        return None
    try:
        direction = int(after_keyword[:comma].strip())
    except ValueError:
        return None
    parsed = _parse_payload(after_keyword[comma + 1:].strip())
    if parsed is None:
        return None
    msg_type, level, data = parsed
    return BroadcastMessage(
        direction=direction, msg_type=msg_type, level=level, data=data
    )
