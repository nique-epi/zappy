/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** WorldQuery — read-only lookups over WorldState (no rendering dependency)
*/

#include "World/WorldQuery.hpp"
#include <string>
#include <vector>

namespace zappy::gui {

const Player* findPlayerById(const WorldState& world, int playerId) {
  for (const auto& player : world.players) {
    if (player.id == playerId) {
      return &player;
    }
  }
  return nullptr;
}

std::vector<TeamProgress> computeTeamProgress(const WorldState& world) {
  std::vector<TeamProgress> progress;
  progress.reserve(world.teams.size());
  for (const std::string& team : world.teams) {
    TeamProgress entry{.name = team, .aliveCount = 0, .maxLevelCount = 0};
    for (const Player& player : world.players) {
      if (!player.alive || player.teamName != team) {
        continue;
      }
      entry.aliveCount++;
      if (player.level >= MAX_PLAYER_LEVEL) {
        entry.maxLevelCount++;
      }
    }
    progress.push_back(entry);
  }
  return progress;
}

}  // namespace zappy::gui
