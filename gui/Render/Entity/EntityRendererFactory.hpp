/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** EntityRendererFactory — instantiates the map entity renderers
*/

#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include "Render/Entity/IEntityRenderer.hpp"

namespace zappy::gui {

/**
 * @brief The kinds of map entity that own a dedicated renderer.
 */
enum class EntityKind : std::uint8_t { Resource, Egg, Player, Incantation };

/**
 * @brief Static factory creating the concrete @ref IEntityRenderer instances.
 *
 * Centralises the mapping from @ref EntityKind to concrete renderer so that
 * adding a new entity kind only touches @ref create and the @ref createAll
 * ordering. Used by @ref WorldRenderer.
 */
class EntityRendererFactory {
 public:
  EntityRendererFactory() = delete;

  /**
   * @brief Create the renderer for a single entity kind.
   */
  static std::unique_ptr<IEntityRenderer> create(EntityKind kind);

  /**
   * @brief Create every entity renderer, in back-to-front draw order.
   */
  static std::vector<std::unique_ptr<IEntityRenderer>> createAll();
};

}  // namespace zappy::gui
