/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** EggRenderer
*/

#pragma once

namespace zappy::gui {

struct WorldState;

class EggRenderer {
 public:
  static void draw3D(const WorldState& world);
};

}  // namespace zappy::gui
