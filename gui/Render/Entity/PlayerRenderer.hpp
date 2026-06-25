/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** PlayerRenderer
*/

#pragma once

#include <raylib.h>

namespace zappy::gui {

struct WorldState;

class PlayerRenderer {
 public:
  static void draw3D(WorldState& world);
  static void drawLevelLabels(const WorldState& world, const Camera3D& camera);
  static void drawBroadcastIcons(WorldState& world, const Camera3D& camera);
};

}  // namespace zappy::gui
