/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Egg — a free slot of a team, materialised as a hatchable egg on a tile
*/

#pragma once

#include <string>

namespace zappy::world {

/**
 * @brief A team slot waiting to hatch into a drone.
 *
 * Each egg belongs to exactly one team, sits on one tile of the world and
 * carries a stable numeric identifier. The number of eggs a team owns at any
 * moment is the number of free slots that team can still accept on a new AI
 * connection or via a Fork action.
 */
struct Egg {
  int id;
  std::string teamName;
  int x;
  int y;
};

}  // namespace zappy::world
