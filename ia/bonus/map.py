"""Mental map of the world built incrementally from Look responses."""
from math import inf
from ia.game.navigation import ORIENTATION_DELTAS, forward_lateral_to_moves
from ia.parsing.look import LookTile
from ia.shared.enum import Direction, Move


def _count_resources(objects: list[str]) -> dict[str, int]:
    """Count occurrences of each object name on a single tile."""
    counts: dict[str, int] = {}
    for obj in objects:
        counts[obj] = counts.get(obj, 0) + 1
    return counts


class WorldMap:
    """Grid of known tiles, updated as the bot explores the world."""

    def __init__(self, width: int, height: int) -> None:
        self.width = width
        self.height = height
        self.world_map: list[list[dict]] = [
            [
                {"resources": {}, "visited": False, "last_seen_turn": -1}
                for _ in range(width)
            ]
            for _ in range(height)
        ]

    def update_from_look(self, tiles: list[LookTile], turn: int) -> None:
        """Write the objects observed on each tile into the mental map."""
        for tile in tiles:
            if tile["coords"] is None:
                continue
            x, y = tile["coords"]
            cell = self.world_map[y % self.height][x % self.width]
            cell["resources"] = _count_resources(tile["objects"])
            cell["visited"] = True
            cell["last_seen_turn"] = turn

    def nearest_resource(
        self,
        pos: tuple[int, int],
        resource: str,
        current_turn: int | None = None,
        max_age: int | None = None,
    ) -> tuple[int, int] | None:
        """Find the nearest known, non-stale tile holding the resource."""
        best_coords: tuple[int, int] | None = None
        best_distance = inf
        for y, row in enumerate(self.world_map):
            for x, cell in enumerate(row):
                if resource not in cell["resources"]:
                    continue
                if self._is_stale(cell, current_turn, max_age):
                    continue
                distance = self.toroidal_distance(pos, (x, y))
                if distance < best_distance:
                    best_distance = distance
                    best_coords = (x, y)
        return best_coords

    def path_to(
        self,
        pos: tuple[int, int],
        orientation: Direction,
        target: tuple[int, int],
    ) -> list[Move]:
        """Build the move sequence from pos/orientation to target."""
        dx = self._shortest_signed_delta(target[0] - pos[0], self.width)
        dy = self._shortest_signed_delta(target[1] - pos[1], self.height)
        forward_x, forward_y, lateral_x, lateral_y = ORIENTATION_DELTAS[
            orientation
        ]
        forward = dx * forward_x + dy * forward_y
        lateral = dx * lateral_x + dy * lateral_y
        return forward_lateral_to_moves(forward, lateral)

    def toroidal_distance(
        self, pos: tuple[int, int], target: tuple[int, int]
    ) -> int:
        """Manhattan distance on the torus, wrapping on each axis."""
        dx = abs(pos[0] - target[0])
        dx = min(dx, self.width - dx)
        dy = abs(pos[1] - target[1])
        dy = min(dy, self.height - dy)
        return dx + dy

    @staticmethod
    def _is_stale(
        cell: dict, current_turn: int | None, max_age: int | None
    ) -> bool:
        """True when the cell was last seen too long ago to be trusted."""
        if current_turn is None or max_age is None:
            return False
        return current_turn - cell["last_seen_turn"] > max_age

    @staticmethod
    def _shortest_signed_delta(delta: int, size: int) -> int:
        """Reduce a signed offset to its shortest toroidal equivalent."""
        reduced = delta % size
        if size - reduced < reduced:
            return reduced - size
        return reduced
