"""Conversion of Look tile indices into bot movement command sequences."""
import math

from ia.shared.enum import Move


def _tile_index_to_offset(index: int) -> tuple[int, int]:
    """Return the (forward, lateral) offset of a Look tile index."""
    row = math.isqrt(index)
    return row, index - row * row - row


def _shortest_lateral(lateral: int, width: int | None) -> int:
    """Reduce a lateral offset to its shortest toroidal equivalent."""
    if width is None:
        return lateral
    if lateral > 0 and width - lateral < lateral:
        return lateral - width
    if lateral < 0 and width + lateral < -lateral:
        return lateral + width
    return lateral


def tile_to_moves(index: int, width: int | None = None) -> list[Move]:
    """Build the ordered move commands to reach a Look tile from the bot."""
    forward, lateral = _tile_index_to_offset(index)
    lateral = _shortest_lateral(lateral, width)
    moves = [Move.FORWARD] * forward
    if lateral < 0:
        moves.append(Move.LEFT)
        moves += [Move.FORWARD] * -lateral
    elif lateral > 0:
        moves.append(Move.RIGHT)
        moves += [Move.FORWARD] * lateral
    return moves
