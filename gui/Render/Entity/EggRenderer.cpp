/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** EggRenderer
*/

#include "Render/Entity/EggRenderer.hpp"
#include <raylib.h>
#include "Render/Entity/EggRendererConfig.hpp"
#include "Render/RenderUtils.hpp"
#include "World/WorldState.hpp"

namespace zappy::gui {

namespace cfg = config;

void EggRenderer::loadAssets() {}

void EggRenderer::draw(WorldState& world) {
  for (const auto& egg : world.eggs) {
    const Vector3 center{tileToWorld(egg.x), cfg::EGG_CENTER_Y,
                         tileToWorld(egg.y)};
    DrawSphereEx(center, cfg::EGG_RADIUS, cfg::EGG_RINGS, cfg::EGG_SLICES,
                 cfg::EGG_FILL_COLOR);
    DrawSphereWires(center, cfg::EGG_RADIUS, cfg::EGG_RINGS, cfg::EGG_SLICES,
                    cfg::EGG_WIRE_COLOR);
  }
}

void EggRenderer::drawOverlay([[maybe_unused]] WorldState& world,
                              [[maybe_unused]] const Camera3D& camera) {}

void EggRenderer::unloadAssets() {}

}  // namespace zappy::gui
