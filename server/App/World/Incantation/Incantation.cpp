/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Incantation — resolve an elevation ritual on a tile
*/

#include "App/World/Incantation/Incantation.hpp"
#include <vector>
#include "App/World/Incantation/ElevationRequirements.hpp"
#include "App/World/Map/Map.hpp"
#include "App/World/Player/Player.hpp"
#include "App/World/Player/PlayerRegistry.hpp"
#include "App/World/Resources/ResourceType.hpp"
#include "App/World/Tile/Tile.hpp"

namespace zappy::world {

namespace {

std::vector<int> gatherSameLevelDrones(const Tile& tile,
                                       const PlayerRegistry& players,
                                       int level) {
  std::vector<int> gathered;
  for (int playerId : tile.players()) {
    const Player* player = players.find(playerId);
    if (player != nullptr && player->level() == level) {
      gathered.push_back(playerId);
    }
  }
  return gathered;
}

bool tileHoldsStones(const Tile& tile, const ElevationRequirement& need) {
  for (ResourceType type : allResourceTypes()) {
    if (tile.quantityOf(type) < need.stones[static_cast<std::size_t>(type)]) {
      return false;
    }
  }
  return true;
}

void consumeStones(Tile& tile, const ElevationRequirement& need) {
  for (ResourceType type : allResourceTypes()) {
    const std::size_t amount = need.stones[static_cast<std::size_t>(type)];
    if (amount > 0) {
      tile.collect(type, amount);
    }
  }
}

}  // namespace

IncantationOutcome getIncantationOutcome(int initiatorId,
                                         PlayerRegistry& players, Map& map) {
  const Player* initiator = players.find(initiatorId);
  if (initiator == nullptr || initiator->level() < minElevationLevel ||
      initiator->level() > maxElevationLevel) {
    return {.succeeded = false, .newLevel = 0, .participants = {}};
  }

  const int level = initiator->level();
  Tile& tile = map.tileAt(initiator->x(), initiator->y());
  const ElevationRequirement& need = elevationRequirement(level);
  const std::vector<int> participants =
      gatherSameLevelDrones(tile, players, level);

  if (participants.size() < need.playersRequired ||
      !tileHoldsStones(tile, need)) {
    return {.succeeded = false, .newLevel = 0, .participants = {}};
  }

  consumeStones(tile, need);
  const int newLevel = level + 1;
  for (int playerId : participants) {
    players.find(playerId)->setLevel(newLevel);
  }
  return {
      .succeeded = true, .newLevel = newLevel, .participants = participants};
}

}  // namespace zappy::world
