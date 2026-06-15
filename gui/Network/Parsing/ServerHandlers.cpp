/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ServerHandlers — handlers for server-level messages
*/

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include "Network/Parsing/HandlerTypes.hpp"
#include "Network/Parsing/Handlers.hpp"
#include "World/WorldState.hpp"

namespace zappy::gui {

void registerServerHandlers(HandlerMap& handlers, WorldState& world) {
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

  handlers["suc"] = [](std::istringstream&) {
    std::cerr << "[suc] unknown command sent by GUI\n";
  };
  handlers["sbp"] = [](std::istringstream&) {
    std::cerr << "[sbp] bad parameter sent by GUI\n";
  };

  for (const auto* cmd : {"pex", "pbc", "pic", "pie", "pfk", "smg"}) {
    handlers[cmd] = [](std::istringstream&) {};
  }
}

}  // namespace zappy::gui
