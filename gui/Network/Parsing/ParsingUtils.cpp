/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ParsingUtils — shared helpers used across all server message handlers
*/

#include "Network/Parsing/ParsingUtils.hpp"
#include <algorithm>
#include <iterator>
#include <sstream>
#include <string>
#include "World/WorldState.hpp"

namespace zappy::gui {

int parseId(std::istringstream& stream) {
  std::string token;
  stream >> token;
  return std::stoi(token.front() == '#' ? token.substr(1) : token);
}

Orientation toOrientation(int value) {
  switch (value) {
    case 2:
      return Orientation::East;
    case 3:
      return Orientation::South;
    case 4:
      return Orientation::West;
    default:
      return Orientation::North;
  }
}

Player* findPlayer(WorldState& world, int playerId) {
  auto found = std::ranges::find_if(
      world.players,
      [playerId](const Player& player) { return player.id == playerId; });
  return found != world.players.end() ? &*found : nullptr;
}

Tile* findTile(WorldState& world, int posX, int posY) {
  if (posY < 0 || posY >= std::ssize(world.tiles)) {
    return nullptr;
  }
  if (posX < 0 || posX >= std::ssize(world.tiles[posY])) {
    return nullptr;
  }
  return &world.tiles[posY][posX];
}

}  // namespace zappy::gui
