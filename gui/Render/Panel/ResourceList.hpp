/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ResourceList — shared renderer for the seven-resource list used by panels
*/

#pragma once

#include <raylib.h>
#include <array>
#include "World/WorldState.hpp"

namespace zappy::gui {

/**
 * @brief Draw the seven resource quantities as labelled, color-swatched rows.
 *
 * Shared by the tile @ref InfoPanel and the @ref PlayerPanel so both render
 * resources identically (swatch + name + right-aligned value, empties dimmed).
 *
 * @param[in] origin       Top-left of the first row (content area, not panel).
 * @param[in] contentWidth Width of the content area (values are right-aligned).
 * @param[in] quantities   The seven resource counts, in protocol order.
 */
void drawResourceList(Vector2 origin, float contentWidth,
                      const std::array<int, RESOURCE_COUNT>& quantities);

}  // namespace zappy::gui
