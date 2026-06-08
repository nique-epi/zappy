/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** MockWorld
*/

#include <vector>
#include "WorldState.hpp"

namespace zappy::gui {

WorldState mockWorld() {
  constexpr int mapSize = 10;

  WorldState world;
  world.width = mapSize;
  world.height = mapSize;

  world.tiles.resize(world.height, std::vector<Tile>(world.width));
  for (int tileY = 0; tileY < world.height; ++tileY) {
    for (int tileX = 0; tileX < world.width; ++tileX) {
      world.tiles[tileY][tileX] = Tile{.x = tileX, .y = tileY, .resources = {}};
    }
  }

  // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
  world.tiles[2][3].resources = {3, 1, 0, 0, 0, 0, 0};
  world.tiles[5][7].resources = {2, 0, 1, 0, 0, 0, 0};
  world.tiles[8][1].resources = {1, 1, 1, 1, 0, 0, 0};

  world.teams = {"Alpha", "Beta"};

  world.players = {
      Player{.id = 1,
             .x = 3,
             .y = 2,
             .orientation = Orientation::North,
             .level = 1,
             .teamName = "Alpha"},
      Player{.id = 2,
             .x = 7,
             .y = 5,
             .orientation = Orientation::East,
             .level = 2,
             .teamName = "Alpha"},
      Player{.id = 3,
             .x = 1,
             .y = 8,
             .orientation = Orientation::South,
             .level = 1,
             .teamName = "Beta"},
  };

  world.eggs = {
      Egg{.id = 1, .playerId = 1, .x = 4, .y = 4},
  };
  // NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

  return world;
}

}  // namespace zappy::gui
