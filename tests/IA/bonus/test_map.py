"""Unit tests for the WorldMap mental map."""
from ia.bonus.map import WorldMap
from ia.shared.enum import Direction, Move


def test_update_from_look_writes_resources_at_coords():
    """
    Given a fresh WorldMap
    When update_from_look records a tile holding linemate
    Then that tile's resources are stored at the matching grid cell
    """
    world_map = WorldMap(10, 10)
    world_map.update_from_look(
        [{"coords": (3, 4), "objects": ["linemate"]}], turn=7
    )
    cell = world_map.world_map[4][3]
    assert cell["resources"] == {"linemate": 1}
    assert cell["visited"] is True
    assert cell["last_seen_turn"] == 7


def test_update_from_look_ignores_tiles_without_coords():
    """
    Given a fresh WorldMap
    When update_from_look receives a tile whose coords is None
    Then no cell is modified
    """
    world_map = WorldMap(10, 10)
    world_map.update_from_look([{"coords": None, "objects": ["food"]}], 1)
    assert all(
        not cell["visited"] for row in world_map.world_map for cell in row
    )


def test_update_from_look_wraps_out_of_bounds_coords():
    """
    Given a fresh WorldMap on a small torus
    When update_from_look records a tile with an out-of-bounds coordinate
    Then the resource is wrapped to the matching in-bounds cell
    """
    world_map = WorldMap(5, 5)
    world_map.update_from_look(
        [{"coords": (6, -1), "objects": ["food"]}], turn=1
    )
    cell = world_map.world_map[4][1]
    assert cell["resources"] == {"food": 1}


def test_update_from_look_overwrites_stale_resource_count():
    """
    Given a WorldMap with a previously recorded tile holding two foods
    When update_from_look records the same tile with no food anymore
    Then the resources dict reflects the latest observation only
    """
    world_map = WorldMap(10, 10)
    world_map.update_from_look(
        [{"coords": (2, 2), "objects": ["food", "food"]}], turn=1
    )
    world_map.update_from_look([{"coords": (2, 2), "objects": []}], turn=2)
    cell = world_map.world_map[2][2]
    assert cell["resources"] == {}
    assert cell["last_seen_turn"] == 2


def test_nearest_resource_returns_closest_known_tile():
    """
    Given a WorldMap with linemate known at two different distances
    When nearest_resource is called from the bot's position
    Then the closer tile's coords are returned
    """
    world_map = WorldMap(10, 10)
    world_map.update_from_look(
        [
            {"coords": (5, 5), "objects": ["linemate"]},
            {"coords": (1, 0), "objects": ["linemate"]},
        ],
        turn=1,
    )
    assert world_map.nearest_resource((0, 0), "linemate") == (1, 0)


def test_nearest_resource_returns_none_when_unknown():
    """
    Given a WorldMap with no recorded resource
    When nearest_resource is called for that resource
    Then None is returned
    """
    world_map = WorldMap(10, 10)
    assert world_map.nearest_resource((0, 0), "linemate") is None


def test_nearest_resource_ignores_stale_tiles():
    """
    Given a WorldMap with a linemate tile recorded long ago
    When nearest_resource is called with a max_age below its staleness
    Then that tile is ignored and None is returned
    """
    world_map = WorldMap(10, 10)
    world_map.update_from_look(
        [{"coords": (1, 0), "objects": ["linemate"]}], turn=1
    )
    result = world_map.nearest_resource(
        (0, 0), "linemate", current_turn=100, max_age=10
    )
    assert result is None


def test_nearest_resource_accepts_fresh_tiles_within_max_age():
    """
    Given a WorldMap with a linemate tile recorded recently
    When nearest_resource is called with a max_age covering that age
    Then the tile is returned
    """
    world_map = WorldMap(10, 10)
    world_map.update_from_look(
        [{"coords": (1, 0), "objects": ["linemate"]}], turn=90
    )
    result = world_map.nearest_resource(
        (0, 0), "linemate", current_turn=100, max_age=10
    )
    assert result == (1, 0)


def test_path_to_straight_ahead_only_moves_forward():
    """
    Given a bot facing north with a target directly ahead
    When path_to is computed
    Then it only contains Forward moves
    """
    world_map = WorldMap(10, 10)
    moves = world_map.path_to((0, 5), Direction.NORTH, (0, 2))
    assert moves == [Move.FORWARD] * 3


def test_path_to_target_behind_turns_around():
    """
    Given a bot facing north with a target directly behind it
    When path_to is computed
    Then it turns around with two Right moves before walking forward
    """
    world_map = WorldMap(10, 10)
    moves = world_map.path_to((0, 2), Direction.NORTH, (0, 5))
    assert moves == [Move.RIGHT, Move.RIGHT] + [Move.FORWARD] * 3


def test_path_to_uses_toroidal_shortcut():
    """
    Given a target reachable faster by wrapping around the torus
    When path_to is computed
    Then it takes the shorter wrapped lateral step
    """
    world_map = WorldMap(5, 5)
    moves = world_map.path_to((0, 0), Direction.NORTH, (4, 0))
    assert moves == [Move.LEFT, Move.FORWARD]


def test_path_to_same_position_is_empty():
    """
    Given a bot already standing on the target tile
    When path_to is computed
    Then no move is required
    """
    world_map = WorldMap(10, 10)
    assert world_map.path_to((3, 3), Direction.NORTH, (3, 3)) == []


def test_toroidal_distance_wraps_on_both_axes():
    """
    Given two tiles closer when wrapping than going the direct way
    When toroidal_distance is computed
    Then it returns the wrapped, shorter distance
    """
    world_map = WorldMap(10, 10)
    assert world_map.toroidal_distance((0, 0), (9, 9)) == 2


def test_update_from_look_wraps_each_axis_with_its_own_size():
    """
    Given a non-square WorldMap (width 8, height 4)
    When update_from_look records a coordinate out of bounds on both axes
    Then x wraps modulo width and y wraps modulo height independently
    """
    world_map = WorldMap(8, 4)
    world_map.update_from_look(
        [{"coords": (9, 5), "objects": ["food"]}], turn=1
    )
    cell = world_map.world_map[1][1]
    assert cell["resources"] == {"food": 1}


def test_toroidal_distance_on_non_square_map_uses_matching_axis_size():
    """
    Given a non-square WorldMap (width 10, height 4)
    When toroidal_distance wraps a tile near the height edge
    Then the wrap uses height, not width, for the y axis
    """
    world_map = WorldMap(10, 4)
    assert world_map.toroidal_distance((0, 0), (0, 3)) == 1
    assert world_map.toroidal_distance((0, 0), (9, 0)) == 1


def test_path_to_uses_toroidal_shortcut_on_non_square_map():
    """
    Given a non-square WorldMap (width 6, height 10)
    When path_to wraps laterally around the narrower width
    Then it takes the shorter wrapped lateral step sized on width
    """
    world_map = WorldMap(6, 10)
    moves = world_map.path_to((0, 0), Direction.NORTH, (5, 0))
    assert moves == [Move.LEFT, Move.FORWARD]


def test_nearest_resource_on_non_square_map_respects_each_axis():
    """
    Given a non-square WorldMap (width 10, height 4) with a known resource
    When nearest_resource is called near the height edge
    Then the wrapped distance on the shorter axis is used to pick it
    """
    world_map = WorldMap(10, 4)
    world_map.update_from_look(
        [{"coords": (0, 3), "objects": ["linemate"]}], turn=1
    )
    assert world_map.nearest_resource((0, 0), "linemate") == (0, 3)
