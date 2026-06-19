"""Unit tests for Connect_nbr reply parsing."""
from ia.parsing.connect import parse_connect_nbr


def test_parse_connect_nbr_returns_zero():
    """
    Given a Connect_nbr reply reporting no free slot
    When parse_connect_nbr is called
    Then it returns 0
    """
    assert parse_connect_nbr("0\n") == 0


def test_parse_connect_nbr_returns_positive_count():
    """
    Given a Connect_nbr reply reporting three free slots
    When parse_connect_nbr is called
    Then it returns 3
    """
    assert parse_connect_nbr("3\n") == 3
