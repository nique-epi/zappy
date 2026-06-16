"""Conversion of Look tile indices into bot movement command sequences."""
import math

from ia.shared.enum import Move

_BROADCAST_DIRECTION_MOVES: dict[int, list[Move]] = {
    1: [Move.FORWARD],
    2: [Move.RIGHT, Move.FORWARD],
    3: [Move.RIGHT],
    4: [Move.RIGHT, Move.RIGHT, Move.FORWARD],
    5: [Move.RIGHT, Move.RIGHT],
    6: [Move.LEFT, Move.LEFT, Move.FORWARD],
    7: [Move.LEFT],
    8: [Move.LEFT, Move.FORWARD],
}


def _tile_index_to_offset(index: int) -> tuple[int, int]:
    """Return the (forward, lateral) offset of a Look tile index."""
    row = math.isqrt(index)
    return row, index - row * row - row


def _shortest_lateral(lateral: int, width: int | None) -> int:
    """Reduce a lateral offset to its shortest toroidal equivalent."""
    if width is None:
        return lateral
    reduced = lateral % width
    if width - reduced < reduced:
        return reduced - width
    return reduced


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


def broadcast_direction_to_moves(direction: int) -> list[Move]:
    """Return the move sequence to step toward broadcast direction K (1–8)."""
    return _BROADCAST_DIRECTION_MOVES.get(direction, [Move.FORWARD])
