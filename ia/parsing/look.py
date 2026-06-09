"""Parsing of the Zappy server response to the ``Look`` command."""
from ia.shared.enum import Direction


def _split_tiles(response: str) -> list[list[str]]:
    """Split a raw Look response into one object list per tile."""
    inner = response.strip()[1:-1].strip()
    if not inner:
        return []
    return [tile.split() for tile in inner.split(",")]


def _tile_index_to_abs_coords(
    index: int,
    pos: tuple[int, int],
    direction: Direction,
    level: int,
) -> tuple[int, int] | None:
    """Map a tile index to absolute coords; stub returning None (ZAP-19)."""
    del index, pos, direction, level


def parse_look(
    response: str,
    pos: tuple[int, int],
    direction: Direction,
    level: int,
) -> list[dict]:
    """Parse a Look response into tiles with their objects and coords."""
    tiles = _split_tiles(response)
    return [
        {
            "coords": _tile_index_to_abs_coords(index, pos, direction, level),
            "objects": tile,
        }
        for index, tile in enumerate(tiles)
    ]
