/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** TeamRegistry — owns every team, hatches and lays eggs
*/

#include "App/World/Team/TeamRegistry.hpp"
#include "App/World/Exceptions/WorldException.hpp"
#include "App/World/Map/Map.hpp"
#include "App/World/Tile/Tile.hpp"

namespace zappy::world {

TeamRegistry::TeamRegistry(const std::vector<std::string>& teamNames) {
  teams_.reserve(teamNames.size());
  names_.reserve(teamNames.size());
  for (const std::string& name : teamNames) {
    if (teams_.emplace(name, Team(name)).second) {
      names_.push_back(name);
    }
  }
}

bool TeamRegistry::contains(const std::string& teamName) const {
  return teams_.find(teamName) != teams_.end();
}

std::size_t TeamRegistry::freeSlots(const std::string& teamName) const {
  return teamOrThrow(teamName).freeSlots();
}

const std::vector<std::string>& TeamRegistry::names() const { return names_; }

void TeamRegistry::seedInitialEggs(Map& map, int slotsPerTeam,
                                   std::mt19937& rng) {
  std::uniform_int_distribution<int> columns(0, map.width() - 1);
  std::uniform_int_distribution<int> rows(0, map.height() - 1);
  for (const std::string& name : names_) {
    Team& team = teamOrThrow(name);
    for (int slot = 0; slot < slotsPerTeam; ++slot) {
      const int x = columns(rng);
      const int y = rows(rng);
      const Egg egg{nextEggId_++, name, x, y};
      team.addEgg(egg);
      map.tileAt(x, y).addEgg(egg.id);
    }
  }
}

Egg TeamRegistry::hatch(const std::string& teamName, Map& map,
                        std::mt19937& rng) {
  Team& team = teamOrThrow(teamName);
  if (team.empty()) {
    throw NoFreeSlotException(teamName);
  }
  std::uniform_int_distribution<std::size_t> pick(0, team.freeSlots() - 1);
  const Egg removed = team.removeEggAt(pick(rng));
  map.tileAt(removed.x, removed.y).removeEgg(removed.id);
  return removed;
}

Egg TeamRegistry::lay(const std::string& teamName, Map& map, int x, int y) {
  Team& team = teamOrThrow(teamName);
  const Egg egg{nextEggId_++, teamName, x, y};
  team.addEgg(egg);
  map.tileAt(x, y).addEgg(egg.id);
  return egg;
}

Team& TeamRegistry::teamOrThrow(const std::string& teamName) {
  auto it = teams_.find(teamName);
  if (it == teams_.end()) {
    throw UnknownTeamException(teamName);
  }
  return it->second;
}

const Team& TeamRegistry::teamOrThrow(const std::string& teamName) const {
  auto it = teams_.find(teamName);
  if (it == teams_.end()) {
    throw UnknownTeamException(teamName);
  }
  return it->second;
}

}  // namespace zappy::world
