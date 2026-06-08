/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Tile — content of a single cell of the world
*/

#include "App/World/Tile/Tile.hpp"
#include <algorithm>

namespace zappy::world {

std::size_t Tile::quantityOf(ResourceType type) const {
  return resources_[static_cast<std::size_t>(type)];
}

void Tile::drop(ResourceType type, std::size_t amount) {
  resources_[static_cast<std::size_t>(type)] += amount;
}

bool Tile::collect(ResourceType type, std::size_t amount) {
  std::size_t& stock = resources_[static_cast<std::size_t>(type)];

  if (stock < amount) {
    return false;
  }
  stock -= amount;
  return true;
}

const std::vector<int>& Tile::players() const { return players_; }

void Tile::addPlayer(int playerId) { players_.push_back(playerId); }

bool Tile::removePlayer(int playerId) {
  const auto found = std::ranges::find(players_, playerId);

  if (found == players_.end()) {
    return false;
  }
  players_.erase(found);
  return true;
}

const std::vector<int>& Tile::eggs() const { return eggs_; }

void Tile::addEgg(int eggId) { eggs_.push_back(eggId); }

bool Tile::removeEgg(int eggId) {
  const auto found = std::ranges::find(eggs_, eggId);

  if (found == eggs_.end()) {
    return false;
  }
  eggs_.erase(found);
  return true;
}

}  // namespace zappy::world
