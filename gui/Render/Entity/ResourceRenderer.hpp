/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ResourceRenderer
*/

#pragma once

namespace zappy::gui {

struct WorldState;

class ResourceRenderer {
 public:
  static void loadModels();
  static void draw3D(const WorldState& world);
  static void unloadModels();
};

}  // namespace zappy::gui
