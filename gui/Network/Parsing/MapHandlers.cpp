/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** MapHandlers — handlers for map-related server messages (msz, bct)
*/

#include <sstream>
#include <vector>
#include "Network/Parsing/HandlerTypes.hpp"
#include "Network/Parsing/Handlers.hpp"
#include "Network/Parsing/ParsingUtils.hpp"
#include "World/WorldState.hpp"

namespace zappy::gui {

namespace {

void handleMsz(WorldState& world, std::istringstream& stream) {
  int width = 0;
  int height = 0;
  stream >> width >> height;
  world.width = width;
  world.height = height;
  world.tiles.assign(height, std::vector<Tile>(width));
  for (int row = 0; row < height; ++row) {
    for (int col = 0; col < width; ++col) {
      world.tiles[row][col] = {.x = col, .y = row, .resources = {}};
    }
  }
}

}  // namespace

void registerMapHandlers(HandlerMap& handlers, WorldState& world) {
  handlers["msz"] = [&world](std::istringstream& stream) {
    handleMsz(world, stream);
  };

  handlers["bct"] = [&world](std::istringstream& stream) {
    int posX = 0;
    int posY = 0;
    stream >> posX >> posY;
    if (auto* tile = findTile(world, posX, posY)) {
      for (auto& resource : tile->resources) {
        stream >> resource;
      }
    }
  };
}

}  // namespace zappy::gui
