/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** EggRenderer
*/

#pragma once

#include <raylib.h>
#include "Render/Entity/IEntityRenderer.hpp"

namespace zappy::gui {

struct WorldState;

class EggRenderer : public IEntityRenderer {
 public:
  void loadAssets() override;
  void draw(WorldState& world) override;
  void drawOverlay(WorldState& world, const Camera3D& camera) override;
  void unloadAssets() override;
};

}  // namespace zappy::gui
