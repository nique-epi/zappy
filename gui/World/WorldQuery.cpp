/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** WorldQuery — read-only lookups over WorldState (no rendering dependency)
*/

#include "World/WorldQuery.hpp"

namespace zappy::gui {

const Player* findPlayerById(const WorldState& world, int playerId) {
  for (const auto& player : world.players) {
    if (player.id == playerId) {
      return &player;
    }
  }
  return nullptr;
}

}  // namespace zappy::gui
