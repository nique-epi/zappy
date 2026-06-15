/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Victory — detect a team that has met the end-of-game elevation condition
*/

#include "App/World/Victory.hpp"
#include <unordered_map>
#include "App/World/Player/Player.hpp"
#include "App/World/Player/PlayerRegistry.hpp"

namespace zappy::world {

std::optional<std::string> findWinningTeam(const PlayerRegistry& players) {
  std::unordered_map<std::string, std::size_t> maxedDronesPerTeam;
  for (const int playerId : players.ids()) {
    const Player* player = players.find(playerId);
    if (player == nullptr || player->level() < maxPlayerLevel) {
      continue;
    }
    const std::size_t reached = ++maxedDronesPerTeam[player->teamName()];
    if (reached >= dronesRequiredToWin) {
      return player->teamName();
    }
  }
  return std::nullopt;
}

}  // namespace zappy::world
