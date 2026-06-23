/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** IncantationRenderer
*/

#pragma once

#include <raylib.h>

namespace zappy::gui {

struct WorldState;

class IncantationRenderer {
 public:
  static void loadTextures();
  static void unloadTextures();
  static void draw3D(WorldState& world);
};

}  // namespace zappy::gui
