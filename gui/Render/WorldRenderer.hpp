/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** WorldRenderer — owns and drives every map entity renderer
*/

#pragma once

#include <raylib.h>
#include <memory>
#include <vector>
#include "Render/Entity/IEntityRenderer.hpp"

namespace zappy::gui {

struct WorldState;

/**
 * @brief Owns the collection of entity renderers and drives their lifecycle.
 *
 * Built from @ref EntityRendererFactory. The constructor loads every
 * renderer's GPU assets and the destructor releases them (RAII), so a
 * WorldRenderer must be constructed only after InitWindow. @ref drawWorld is
 * called inside BeginMode3D/EndMode3D and @ref drawOverlay just after.
 */
class WorldRenderer {
 public:
  WorldRenderer();
  ~WorldRenderer();

  WorldRenderer(const WorldRenderer&) = delete;
  WorldRenderer& operator=(const WorldRenderer&) = delete;
  WorldRenderer(WorldRenderer&&) = delete;
  WorldRenderer& operator=(WorldRenderer&&) = delete;

  /**
   * @brief Draw every entity kind in back-to-front order, inside BeginMode3D.
   */
  void drawWorld(WorldState& world);

  /**
   * @brief Draw every entity kind's 2D overlay, after EndMode3D.
   */
  void drawOverlay(WorldState& world, const Camera3D& camera);

 private:
  std::vector<std::unique_ptr<IEntityRenderer>> renderers_;
};

}  // namespace zappy::gui
