/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** PlayerRegistry — single owner of every living drone in the world
*/

#include "App/World/Player/PlayerRegistry.hpp"
#include <algorithm>
#include "App/World/Map/Map.hpp"
#include "App/World/Tile/Tile.hpp"

namespace zappy::world {

std::size_t PlayerRegistry::size() const { return players_.size(); }

bool PlayerRegistry::contains(int playerId) const {
  return players_.find(playerId) != players_.end();
}

const Player* PlayerRegistry::find(int playerId) const {
  auto iter = players_.find(playerId);
  if (iter == players_.end()) {
    return nullptr;
  }
  return iter->second.get();
}

Player* PlayerRegistry::find(int playerId) {
  auto iter = players_.find(playerId);
  if (iter == players_.end()) {
    return nullptr;
  }
  return iter->second.get();
}

Player& PlayerRegistry::spawn(const std::string& teamName, int x, int y,
                              Direction direction, Map& map) {
  const int playerId = nextPlayerId_++;
  auto player = std::make_unique<Player>(playerId, teamName, x, y, direction);
  Player& reference = *player;
  players_.emplace(playerId, std::move(player));
  insertionOrder_.push_back(playerId);
  map.tileAt(x, y).addPlayer(playerId);
  return reference;
}

Direction PlayerRegistry::randomDirection(std::mt19937& rng) {
  std::uniform_int_distribution<int> pick(0, 3);
  switch (pick(rng)) {
    case 0:
      return Direction::North;
    case 1:
      return Direction::East;
    case 2:
      return Direction::South;
    default:
      return Direction::West;
  }
}

bool PlayerRegistry::remove(int playerId, Map& map) {
  auto iter = players_.find(playerId);
  if (iter == players_.end()) {
    return false;
  }
  const Player& player = *iter->second;
  map.tileAt(player.x(), player.y()).removePlayer(playerId);
  players_.erase(iter);
  auto orderIter =
      std::find(insertionOrder_.begin(), insertionOrder_.end(), playerId);
  if (orderIter != insertionOrder_.end()) {
    insertionOrder_.erase(orderIter);
  }
  return true;
}

std::vector<int> PlayerRegistry::ids() const { return insertionOrder_; }

}  // namespace zappy::world
