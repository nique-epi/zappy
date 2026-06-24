"""Unit tests for the Look response parser."""
from ia.parsing.look import parse_look, _split_tiles
from ia.shared.enum import Direction


def test_split_tiles_current_tile_is_index_zero():
    """
    Given a Look response whose first tile holds the player
    When the response is split into tiles
    Then index 0 carries the bot's current tile objects
    """
    tiles = _split_tiles("[player,,,thystame]")
    assert tiles[0] == ["player"]


def test_split_tiles_empty_tile_is_empty_list():
    """
    Given a Look response with a tile holding no object
    When the response is split into tiles
    Then that tile is represented by an empty list
    """
    tiles = _split_tiles("[player,,food]")
    assert tiles[1] == []


def test_split_tiles_comma_without_space():
    """
    Given a Look response using bare commas as separators
    When the response is split into tiles
    Then each comma-delimited tile is parsed independently
    """
    tiles = _split_tiles("[player,linemate]")
    assert tiles == [["player"], ["linemate"]]


def test_split_tiles_comma_followed_by_space():
    """
    Given a Look response using a comma followed by a space
    When the response is split into tiles
    Then the leading space is ignored and tiles match the bare-comma form
    """
    tiles = _split_tiles("[player, linemate]")
    assert tiles == [["player"], ["linemate"]]


def test_split_tiles_multiple_objects_on_one_tile():
    """
    Given a Look response with two objects on the same tile
    When the response is split into tiles
    Then both space-separated objects are kept in that tile's list
    """
    tiles = _split_tiles("[player food,linemate]")
    assert tiles[0] == ["player", "food"]


def test_split_tiles_subject_example():
    """
    Given the canonical Look response shape from the subject
    When the response is split into tiles
    Then every tile is parsed in order, empty ones as empty lists
    """
    tiles = _split_tiles("[player,,,thystame,,food]")
    assert tiles == [["player"], [], [], ["thystame"], [], ["food"]]


def test_split_tiles_empty_response():
    """
    Given an empty Look response carrying no tile
    When the response is split into tiles
    Then no tile is produced
    """
    assert _split_tiles("[]") == []


def test_parse_look_wraps_each_tile_in_a_dict():
    """
    Given a Look response
    When it is parsed
    Then each tile becomes a dict exposing coords and objects
    """
    result = parse_look("[player,food]", (0, 0), Direction.NORTH, 1)
    assert result[0] == {"coords": (0, 0), "objects": ["player"]}


def test_parse_look_objects_match_split_tiles():
    """
    Given a Look response with several tiles
    When it is parsed
    Then each dict's objects equal the corresponding split tile
    """
    response = "[player,,linemate food]"
    result = parse_look(response, (3, 4), Direction.EAST, 2)
    objects = [tile["objects"] for tile in result]
    assert objects == _split_tiles(response)


def test_parse_look_coords_are_absolute_unwrapped_positions():
    """
    Given a Look response from a bot facing south at (1, 1)
    When it is parsed
    Then each tile's coords is its absolute, unwrapped position
    """
    result = parse_look("[player,,food]", (1, 1), Direction.SOUTH, 1)
    coords = [tile["coords"] for tile in result]
    assert coords == [(1, 1), (2, 2), (1, 2)]
