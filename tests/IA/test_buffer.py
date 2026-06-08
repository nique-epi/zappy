"""
Unit tests for the Buffer class.
"""
from ia.network.buffer import Buffer


def test_pop_line_empty():
    """
    Given a fresh empty buffer
    When checking for a line and popping
    Then no line is available and pop_line returns None
    """
    buf = Buffer()
    assert buf.has_line() is False
    assert buf.pop_line() is None


def test_pop_line_single():
    """
    Given a buffer fed with a single newline-terminated line
    When pop_line is called
    Then it returns the line without its newline and nothing remains
    """
    buf = Buffer()
    buf.feed("Forward\n")
    assert buf.has_line() is True
    assert buf.pop_line() == "Forward"
    assert buf.pop_line() is None


def test_pop_line_multiple_in_one_feed():
    """
    Given a buffer fed with several lines in a single chunk
    When pop_line is called repeatedly
    Then each complete line is returned in order, then None
    """
    buf = Buffer()
    buf.feed("a\nb\nc\n")
    assert buf.pop_line() == "a"
    assert buf.pop_line() == "b"
    assert buf.pop_line() == "c"
    assert buf.pop_line() is None


def test_pop_line_partial_then_completed():
    """
    Given a buffer fed with a fragment without a newline
    When more data completing the line is fed
    Then pop_line returns the reassembled line
    """
    buf = Buffer()
    buf.feed("ab")
    assert buf.has_line() is False
    assert buf.pop_line() is None
    buf.feed("c\n")
    assert buf.pop_line() == "abc"


def test_pop_line_keeps_remainder():
    """
    Given a buffer fed with one full line followed by a partial one
    When the full line is popped
    Then the partial remainder stays buffered until its newline arrives
    """
    buf = Buffer()
    buf.feed("ok\npartial")
    assert buf.pop_line() == "ok"
    assert buf.has_line() is False
    assert buf.pop_line() is None
    buf.feed("\n")
    assert buf.pop_line() == "partial"


def test_pop_line_strips_carriage_return():
    """
    Given a buffer fed with a CRLF-terminated line
    When pop_line is called
    Then only the trailing carriage return is stripped
    """
    buf = Buffer()
    buf.feed("hello\r\n")
    assert buf.pop_line() == "hello"


def test_pop_line_preserves_significant_spaces():
    """
    Given a buffer fed with a line carrying leading and trailing spaces
    When pop_line is called
    Then only the line terminator is removed and the spaces are preserved
    """
    buf = Buffer()
    buf.feed("  message 0,  hi there  \r\n")
    assert buf.pop_line() == "  message 0,  hi there  "
