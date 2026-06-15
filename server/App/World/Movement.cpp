/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Movement — advance a drone across the toroidal world
*/

#include "App/World/Movement.hpp"
#include "App/World/Map/Map.hpp"
#include "App/World/Player/Direction.hpp"
#include "App/World/Player/Player.hpp"
#include "App/World/Tile/Tile.hpp"

namespace zappy::world {

void moveForward(Player& player, Map& map) {
  const DirectionOffset offset = directionOffset(player.direction());
  const int destinationColumn = Map::wrap(player.x() + offset.dx, map.width());
  const int destinationRow = Map::wrap(player.y() + offset.dy, map.height());
  map.tileAt(player.x(), player.y()).removePlayer(player.id());
  map.tileAt(destinationColumn, destinationRow).addPlayer(player.id());
  player.setPosition(destinationColumn, destinationRow);
}

}  // namespace zappy::world
