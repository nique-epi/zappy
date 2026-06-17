/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** TileGridRenderer
*/

#pragma once

namespace zappy::gui {

struct WorldState;

class TileGridRenderer {
 public:
  static void draw(const WorldState& world);
};

}  // namespace zappy::gui
