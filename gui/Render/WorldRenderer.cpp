/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** WorldRenderer — owns and drives every map entity renderer
*/

#include "Render/WorldRenderer.hpp"
#include <raylib.h>
#include "Render/Entity/EntityRendererFactory.hpp"
#include "World/WorldState.hpp"

namespace zappy::gui {

WorldRenderer::WorldRenderer()
    : renderers_(EntityRendererFactory::createAll()) {
  for (auto& renderer : renderers_) {
    renderer->loadAssets();
  }
}

WorldRenderer::~WorldRenderer() {
  for (auto& renderer : renderers_) {
    renderer->unloadAssets();
  }
}

void WorldRenderer::drawWorld(WorldState& world) {
  for (auto& renderer : renderers_) {
    renderer->draw(world);
  }
}

void WorldRenderer::drawOverlay(WorldState& world, const Camera3D& camera) {
  for (auto& renderer : renderers_) {
    renderer->drawOverlay(world, camera);
  }
}

}  // namespace zappy::gui
