"""Parsing of the Zappy server response to the ``Look`` command."""
from typing import TypedDict

from ia.game.navigation import ORIENTATION_DELTAS, tile_index_to_offset
from ia.shared.enum import Direction


class LookTile(TypedDict):
    """One tile entry produced by ``parse_look``."""
    coords: tuple[int, int] | None
    objects: list[str]


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
    """Map a Look tile index to the bot's absolute (unwrapped) coords."""
    del level
    forward, lateral = tile_index_to_offset(index)
    forward_dx, forward_dy, lateral_dx, lateral_dy = ORIENTATION_DELTAS[
        direction
    ]
    dx = forward * forward_dx + lateral * lateral_dx
    dy = forward * forward_dy + lateral * lateral_dy
    return pos[0] + dx, pos[1] + dy


def parse_look(
    response: str,
    pos: tuple[int, int],
    direction: Direction,
    level: int,
) -> list[LookTile]:
    """Parse a Look response into tiles with their objects and coords."""
    tiles = _split_tiles(response)
    return [
        {
            "coords": _tile_index_to_abs_coords(index, pos, direction, level),
            "objects": tile,
        }
        for index, tile in enumerate(tiles)
    ]
