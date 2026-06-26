/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ServerHandlers — handlers for server-level messages
*/

#include <algorithm>
#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include "Network/Parsing/HandlerTypes.hpp"
#include "Network/Parsing/Handlers.hpp"
#include "World/WorldState.hpp"

namespace zappy::gui {

namespace {

void registerGameHandlers(HandlerMap& handlers, WorldState& world) {
  handlers["tna"] = [&world](std::istringstream& stream) {
    std::string name;
    stream >> name;
    if (std::ranges::find(world.teams, name) == world.teams.end()) {
      world.teams.push_back(name);
    }
  };

  const auto setTimeUnit = [&world](std::istringstream& stream) {
    stream >> world.timeUnit;
  };
  handlers["sgt"] = handlers["sst"] = setTimeUnit;

  handlers["seg"] = [&world](std::istringstream& stream) {
    world.gameOver = true;
    stream >> world.winnerTeam;
  };
}

void registerDiagnosticHandlers(HandlerMap& handlers) {
  handlers["suc"] = [](std::istringstream&) {
    std::cerr << "[suc] unknown command sent by GUI\n";
  };
  handlers["sbp"] = [](std::istringstream&) {
    std::cerr << "[sbp] bad parameter sent by GUI\n";
  };

  for (const auto* cmd : {"pfk", "smg"}) {
    handlers[cmd] = [](std::istringstream&) {};
  }
}

void registerIncantationHandlers(HandlerMap& handlers, WorldState& world) {
  handlers["pic"] = [&world](std::istringstream& stream) {
    Incantation incantation;
    stream >> incantation.x >> incantation.y >> incantation.level;
    const bool alreadyActive = std::ranges::any_of(
        world.activeIncantations, [&](const Incantation& inc) {
          return inc.x == incantation.x && inc.y == incantation.y;
        });
    if (!alreadyActive) {
      world.activeIncantations.push_back(incantation);
    }
  };

  handlers["pie"] = [&world](std::istringstream& stream) {
    int posX = 0;
    int posY = 0;
    int result = 0;
    stream >> posX >> posY >> result;
    const auto it = std::ranges::find_if(
        world.activeIncantations,
        [&](const Incantation& inc) { return inc.x == posX && inc.y == posY; });
    if (it != world.activeIncantations.end()) {
      world.finishedIncantations.push_back(
          {posX, posY, result == 1, std::chrono::steady_clock::now()});
      world.activeIncantations.erase(it);
    }
  };
}

}  // namespace

void registerServerHandlers(HandlerMap& handlers, WorldState& world) {
  registerGameHandlers(handlers, world);
  registerDiagnosticHandlers(handlers);
  registerIncantationHandlers(handlers, world);
}

}  // namespace zappy::gui
