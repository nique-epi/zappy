/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** IEntityRenderer — interface for a map entity renderer
*/

#pragma once

#include <raylib.h>

namespace zappy::gui {

struct WorldState;

/**
 * @brief Interface for a renderer of one kind of map entity.
 *
 * Each concrete renderer owns the GPU assets (models, textures) for its entity
 * kind and draws every instance of that kind found in the @ref WorldState.
 * The lifecycle is RAII-friendly: loadAssets() acquires GPU resources (call
 * after InitWindow) and unloadAssets() releases them.
 *
 * Implemented by EggRenderer, PlayerRenderer, ResourceRenderer and
 * IncantationRenderer; instantiated by @ref EntityRendererFactory and owned by
 * @ref WorldRenderer.
 */
class IEntityRenderer {
 public:
  IEntityRenderer() = default;
  virtual ~IEntityRenderer() = default;
  IEntityRenderer(const IEntityRenderer&) = delete;
  IEntityRenderer& operator=(const IEntityRenderer&) = delete;
  IEntityRenderer(IEntityRenderer&&) = delete;
  IEntityRenderer& operator=(IEntityRenderer&&) = delete;

  /**
   * @brief Acquire the GPU assets for this entity kind.
   *
   * Must be called once, after InitWindow, before the first @ref draw.
   */
  virtual void loadAssets() = 0;

  /**
   * @brief Draw every instance of this entity kind, inside BeginMode3D.
   *
   * @param world Mutable world; some renderers purge their expired animations.
   */
  virtual void draw(WorldState& world) = 0;

  /**
   * @brief Draw the 2D screen overlay for this entity kind, after EndMode3D.
   *
   * @param world  Read-only world snapshot.
   * @param camera Active 3D camera, used to project world positions to screen.
   */
  virtual void drawOverlay(const WorldState& world, const Camera3D& camera) = 0;

  /**
   * @brief Release the GPU assets acquired by @ref loadAssets.
   */
  virtual void unloadAssets() = 0;
};

}  // namespace zappy::gui
