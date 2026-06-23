"""Unit tests for the Look tile index to movement command conversion."""
from ia.game.navigation import tile_to_moves, tile_index_to_offset
from ia.shared.enum import Move


def test_offset_current_tile_is_origin():
    """
    Given the bot's own tile at Look index 0
    When its bot-frame offset is computed
    Then it sits at zero forward and zero lateral
    """
    assert tile_index_to_offset(0) == (0, 0)


def test_offset_row_grows_with_perfect_squares():
    """
    Given the left, centre and right tiles of the second vision row
    When their bot-frame offsets are computed
    Then they share the same forward depth and span lateral -2..+2
    """
    assert tile_index_to_offset(4) == (2, -2)
    assert tile_index_to_offset(6) == (2, 0)
    assert tile_index_to_offset(8) == (2, 2)


def test_current_tile_needs_no_move():
    """
    Given the bot already standing on the target tile (index 0)
    When the move sequence is built
    Then it is empty
    """
    assert tile_to_moves(0) == []


def test_same_column_only_walks_forward():
    """
    Given a tile directly ahead of the bot (centre of a vision row)
    When the move sequence is built
    Then it only walks forward and never turns
    """
    assert tile_to_moves(2) == [Move.FORWARD]
    assert tile_to_moves(6) == [Move.FORWARD, Move.FORWARD]


def test_tile_on_the_left_turns_left():
    """
    Given a tile on the bot's left side of the first vision row
    When the move sequence is built
    Then the bot walks forward, turns left and steps onto the tile
    """
    assert tile_to_moves(1) == [Move.FORWARD, Move.LEFT, Move.FORWARD]


def test_tile_on_the_right_turns_right():
    """
    Given a tile on the bot's right side of the first vision row
    When the move sequence is built
    Then the bot walks forward, turns right and steps onto the tile
    """
    assert tile_to_moves(3) == [Move.FORWARD, Move.RIGHT, Move.FORWARD]


def test_maximum_vision_right_corner_tile():
    """
    Given the far-right corner tile of a level-3 vision triangle (index 15)
    When the move sequence is built
    Then the bot walks the full depth, turns right and walks the full width
    """
    assert tile_to_moves(15) == (
        [Move.FORWARD] * 3 + [Move.RIGHT] + [Move.FORWARD] * 3
    )


def test_maximum_vision_left_corner_tile():
    """
    Given the far-left corner tile of a level-3 vision triangle (index 9)
    When the move sequence is built
    Then the bot walks the full depth, turns left and walks the full width
    """
    assert tile_to_moves(9) == (
        [Move.FORWARD] * 3 + [Move.LEFT] + [Move.FORWARD] * 3
    )


def test_wrapping_takes_the_short_lateral_way():
    """
    Given a tile two columns right on a width-3 toroidal map (index 8)
    When the move sequence is built with the map width
    Then the bot wraps and turns left for a single step instead of two right
    """
    assert tile_to_moves(8, width=3) == (
        [Move.FORWARD] * 2 + [Move.LEFT] + [Move.FORWARD]
    )


def test_wrapping_reduces_offsets_larger_than_width():
    """
    Given a tile whose lateral offset exceeds the map width (index 35)
    When the move sequence is built with a width-3 toroidal map
    Then the offset is fully reduced to the shortest signed step
    """
    assert tile_to_moves(35, width=3) == (
        [Move.FORWARD] * 5 + [Move.LEFT] + [Move.FORWARD]
    )


def test_without_width_lateral_is_not_wrapped():
    """
    Given the same tile but no map width supplied
    When the move sequence is built
    Then the lateral offset is taken at face value with no wrapping
    """
    assert tile_to_moves(8) == (
        [Move.FORWARD] * 2 + [Move.RIGHT] + [Move.FORWARD] * 2
    )
