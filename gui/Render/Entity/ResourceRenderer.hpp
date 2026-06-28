/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ResourceRenderer
*/

#pragma once

#include <raylib.h>
#include <vector>
#include "Render/Entity/IEntityRenderer.hpp"

namespace zappy::gui {

struct WorldState;

class ResourceRenderer : public IEntityRenderer {
 public:
  void loadAssets() override;
  void draw(WorldState& world) override;
  void drawOverlay(WorldState& world, const Camera3D& camera) override;
  void unloadAssets() override;

 private:
  Texture2D colormap_{};
  std::vector<Model> foodModels_;
  std::vector<Model> gemModels_;
};

}  // namespace zappy::gui
