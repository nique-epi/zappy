/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Ejection — push every other drone off a tile and wipe the eggs on it
*/

#include "App/World/Ejection.hpp"
#include <vector>
#include "App/World/Map/Map.hpp"
#include "App/World/Player/Direction.hpp"
#include "App/World/Player/Player.hpp"
#include "App/World/Player/PlayerRegistry.hpp"
#include "App/World/Team/TeamRegistry.hpp"
#include "App/World/Tile/Tile.hpp"

namespace zappy::world {

namespace {

int wrap(int value, int modulus) {
  return ((value % modulus) + modulus) % modulus;
}

}  // namespace

EjectOutcome ejectFromTile(int ejectorId, PlayerRegistry& players, Map& map,
                           TeamRegistry& teams) {
  EjectOutcome outcome{.ejected = {}, .eggsDestroyed = 0};
  const Player* ejector = players.find(ejectorId);
  if (ejector == nullptr) {
    return outcome;
  }
  const int originX = ejector->x();
  const int originY = ejector->y();
  const Direction push = ejector->direction();
  const DirectionOffset offset = directionOffset(push);
  Tile& origin = map.tileAt(originX, originY);

  const std::vector<int> occupants = origin.players();
  for (const int occupantId : occupants) {
    if (occupantId == ejectorId) {
      continue;
    }
    Player* victim = players.find(occupantId);
    if (victim == nullptr) {
      continue;
    }
    const int destinationX = wrap(originX + offset.dx, map.width());
    const int destinationY = wrap(originY + offset.dy, map.height());
    origin.removePlayer(occupantId);
    map.tileAt(destinationX, destinationY).addPlayer(occupantId);
    victim->setPosition(destinationX, destinationY);
    outcome.ejected.push_back(
        {.playerId = occupantId,
         .code = ejectionCode(push, victim->direction())});
  }

  const std::vector<int> doomedEggs = origin.eggs();
  for (const int eggId : doomedEggs) {
    teams.removeEgg(eggId);
    origin.removeEgg(eggId);
    ++outcome.eggsDestroyed;
  }
  return outcome;
}

}  // namespace zappy::world
