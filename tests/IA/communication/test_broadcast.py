"""Unit tests for broadcast message formatting and parsing."""
from ia.communication.broadcast import (
    BroadcastMessage,
    MessageType,
    format_message,
    parse_broadcast,
)


def test_format_message_without_data():
    """
    Given a message type and a level with no extra data
    When format_message is called
    Then it returns a ZAPPY-prefixed colon-separated string with an empty data field
    """
    result = format_message(MessageType.LEAD, 2)
    assert result == "ZAPPY:LEAD:2:"


def test_format_message_with_data():
    """
    Given a message type, a level, and a non-empty data string
    When format_message is called
    Then the data field is appended after the level
    """
    result = format_message(MessageType.FORK_NEEDED, 3, "2")
    assert result == "ZAPPY:FORK_NEEDED:3:2"


def test_format_message_all_types():
    """
    Given each MessageType value
    When format_message is called
    Then the type name appears verbatim in the payload
    """
    for msg_type in MessageType:
        result = format_message(msg_type, 1)
        assert msg_type.value in result


def test_parse_broadcast_valid_lead():
    """
    Given a well-formed LEAD broadcast line from the server
    When parse_broadcast is called
    Then it returns a BroadcastMessage with all fields correctly parsed
    """
    result = parse_broadcast("message 3, ZAPPY:LEAD:2:")
    assert result == BroadcastMessage(direction=3, msg_type=MessageType.LEAD, level=2, data="")


def test_parse_broadcast_valid_ready():
    """
    Given a well-formed READY broadcast line
    When parse_broadcast is called
    Then it returns a BroadcastMessage with direction, type and level set
    """
    result = parse_broadcast("message 1, ZAPPY:READY:1:")
    assert result == BroadcastMessage(direction=1, msg_type=MessageType.READY, level=1, data="")


def test_parse_broadcast_valid_join():
    """
    Given a well-formed JOIN broadcast line
    When parse_broadcast is called
    Then it returns a BroadcastMessage with msg_type JOIN
    """
    result = parse_broadcast("message 7, ZAPPY:JOIN:4:")
    assert result is not None
    assert result.msg_type == MessageType.JOIN
    assert result.direction == 7
    assert result.level == 4


def test_parse_broadcast_valid_fork_needed_with_data():
    """
    Given a FORK_NEEDED broadcast line containing a data field
    When parse_broadcast is called
    Then the data field is preserved in the returned message
    """
    result = parse_broadcast("message 2, ZAPPY:FORK_NEEDED:3:2")
    assert result is not None
    assert result.msg_type == MessageType.FORK_NEEDED
    assert result.data == "2"


def test_parse_broadcast_foreign_prefix_returns_none():
    """
    Given a broadcast line whose payload does not start with ZAPPY
    When parse_broadcast is called
    Then it returns None
    """
    result = parse_broadcast("message 5, ENEMY:LEAD:2:")
    assert result is None


def test_parse_broadcast_random_text_returns_none():
    """
    Given a broadcast line with an unstructured text payload
    When parse_broadcast is called
    Then it returns None
    """
    result = parse_broadcast("message 4, hello world")
    assert result is None


def test_parse_broadcast_unknown_type_returns_none():
    """
    Given a broadcast line with an unrecognised message type
    When parse_broadcast is called
    Then it returns None
    """
    result = parse_broadcast("message 1, ZAPPY:UNKNOWN:2:")
    assert result is None


def test_parse_broadcast_non_integer_level_returns_none():
    """
    Given a broadcast line where the level field is not a number
    When parse_broadcast is called
    Then it returns None
    """
    result = parse_broadcast("message 1, ZAPPY:LEAD:abc:")
    assert result is None


def test_parse_broadcast_wrong_prefix_returns_none():
    """
    Given a line that does not start with 'message '
    When parse_broadcast is called
    Then it returns None
    """
    result = parse_broadcast("eject: 3")
    assert result is None


def test_parse_broadcast_direction_zero_same_tile():
    """
    Given a broadcast emitted from the same tile (direction 0)
    When parse_broadcast is called
    Then direction is 0 and the message is otherwise valid
    """
    result = parse_broadcast("message 0, ZAPPY:READY:1:")
    assert result is not None
    assert result.direction == 0
