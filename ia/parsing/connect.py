"""Parsing of the Connect_nbr server reply."""


def parse_connect_nbr(response: str) -> int:
    """Return the number of free team slots from a Connect_nbr reply."""
    return int(response.strip())
