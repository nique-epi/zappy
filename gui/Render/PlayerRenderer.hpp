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
  PlayerRenderer();
  ~PlayerRenderer();
  PlayerRenderer(const PlayerRenderer&) = delete;
  PlayerRenderer& operator=(const PlayerRenderer&) = delete;

  void draw3D(const WorldState& world);
  static void drawLevelLabels(const WorldState& world, const Camera3D& camera);

 private:
  Model model_;
  ModelAnimation* animations_{nullptr};
  int animCount_{0};
  int animFrame_{0};
};

}  // namespace zappy::gui
