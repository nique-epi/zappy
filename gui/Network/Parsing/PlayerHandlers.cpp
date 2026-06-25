/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** PlayerHandlers — handlers for player-related server messages
*/

#include <algorithm>
#include <chrono>
#include <sstream>
#include <string>
#include "Network/Parsing/HandlerTypes.hpp"
#include "Network/Parsing/Handlers.hpp"
#include "Network/Parsing/ParsingUtils.hpp"
#include "World/WorldState.hpp"

namespace zappy::gui {

void registerPlayerHandlers(HandlerMap& handlers, WorldState& world) {
  handlers["pnw"] = [&world](std::istringstream& stream) {
    int posX = 0;
    int posY = 0;
    int orientationValue = 0;
    int level = 0;
    std::string team;
    const int playerId =  // NOLINT(cppcoreguidelines-init-variables)
        parseId(stream);
    stream >> posX >> posY >> orientationValue >> level >> team;
    world.players.push_back(
        Player{.id = playerId,
               .x = posX,
               .y = posY,
               .orientation = toOrientation(orientationValue),
               .level = level,
               .teamName = team,
               .inventory = {}});
  };

  handlers["ppo"] = [&world](std::istringstream& stream) {
    int posX = 0;
    int posY = 0;
    int orientationValue = 0;
    auto* targetPlayer = findPlayer(world, parseId(stream));
    stream >> posX >> posY >> orientationValue;
    if (targetPlayer != nullptr) {
      targetPlayer->x = posX;
      targetPlayer->y = posY;
      targetPlayer->orientation = toOrientation(orientationValue);
    }
  };

  handlers["plv"] = [&world](std::istringstream& stream) {
    int level = 0;
    auto* targetPlayer = findPlayer(world, parseId(stream));
    stream >> level;
    if (targetPlayer != nullptr) {
      targetPlayer->level = level;
    }
  };

  handlers["pin"] = [&world](std::istringstream& stream) {
    int posX = 0;
    int posY = 0;
    auto* targetPlayer = findPlayer(world, parseId(stream));
    stream >> posX >> posY;
    if (targetPlayer == nullptr) {
      return;
    }
    targetPlayer->x = posX;
    targetPlayer->y = posY;
    for (auto& slot : targetPlayer->inventory) {
      stream >> slot;
    }
  };

  handlers["pex"] = [&world](std::istringstream& stream) {
    const int playerId = parseId(stream);
    const bool alreadyEjected = std::ranges::any_of(
        world.playerEjections, [&](const PlayerEjection& ejection) {
          return ejection.playerId == playerId;
        });
    if (!alreadyEjected) {
      world.playerEjections.push_back(
          {.playerId = playerId,
           .startTime = std::chrono::steady_clock::now()});
    }
  };

  handlers["pdi"] = [&world](std::istringstream& stream) {
    auto* targetPlayer = findPlayer(world, parseId(stream));
    if (targetPlayer != nullptr) {
      targetPlayer->alive = false;
    }
  };

  handlers["pdr"] = [&world](std::istringstream& stream) {
    int resourceIndex = 0;
    auto* targetPlayer = findPlayer(world, parseId(stream));
    stream >> resourceIndex;
    if (targetPlayer == nullptr) {
      return;
    }
    if (auto* tile = findTile(world, targetPlayer->x, targetPlayer->y)) {
      tile->resources[resourceIndex]++;
      targetPlayer->inventory[resourceIndex]--;
    }
  };

  handlers["pgt"] = [&world](std::istringstream& stream) {
    int resourceIndex = 0;
    auto* targetPlayer = findPlayer(world, parseId(stream));
    stream >> resourceIndex;
    if (targetPlayer == nullptr) {
      return;
    }
    if (auto* tile = findTile(world, targetPlayer->x, targetPlayer->y)) {
      tile->resources[resourceIndex]--;
      targetPlayer->inventory[resourceIndex]++;
    }
  };
}

}  // namespace zappy::gui
