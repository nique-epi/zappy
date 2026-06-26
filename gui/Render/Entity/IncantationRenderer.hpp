/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** IncantationRenderer
*/

#pragma once

#include <raylib.h>
#include <vector>
#include "Render/Entity/IEntityRenderer.hpp"

namespace zappy::gui {

struct WorldState;

class IncantationRenderer : public IEntityRenderer {
 public:
  void loadAssets() override;
  void draw(WorldState& world) override;
  void drawOverlay(WorldState& world, const Camera3D& camera) override;
  void unloadAssets() override;

 private:
  std::vector<Texture2D> frames_;
  Model planeModel_{};
};

}  // namespace zappy::gui
