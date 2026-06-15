/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ResourceTransfer — move resource units between a tile and a drone inventory
*/

#include "App/World/ResourceTransfer.hpp"
#include "App/World/Player/Player.hpp"
#include "App/World/Tile/Tile.hpp"

namespace zappy::world {

bool takeResource(Player& player, Tile& tile, ResourceType type) {
  if (!tile.collect(type)) {
    return false;
  }
  player.inventory().add(type);
  return true;
}

bool dropResource(Player& player, Tile& tile, ResourceType type) {
  if (!player.inventory().consume(type)) {
    return false;
  }
  tile.drop(type);
  return true;
}

}  // namespace zappy::world
