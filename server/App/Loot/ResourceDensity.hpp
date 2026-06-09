/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ResourceDensity — per-kind spawn density and target stock on the world floor
*/

#pragma once

#include <cstddef>
#include "App/World/Resources/ResourceType.hpp"

namespace zappy::loot {

/// Game-time units between two automatic resource replenishments.
constexpr int refillTimeUnits = 20;

/**
 * @brief Target number of units of @p type the whole map should hold.
 *
 * Applies the subject formula `map_width * map_height * density` with the
 * fixed per-kind densities (food 0.5, linemate 0.3, deraumere 0.15, sibur 0.1,
 * mendiane 0.1, phiras 0.08, thystame 0.05). The computation is done in integer
 * arithmetic (densities expressed in hundredths) so the result is exact and
 * platform-independent — a floating-point `0.3 * 100` would drift to 29 instead
 * of 30. The result is clamped to at least one, guaranteeing every kind is
 * present even on a tiny map.
 *
 * @param[in] type   The resource kind whose target stock is wanted.
 * @param[in] width  Map width in tiles; must be > 0.
 * @param[in] height Map height in tiles; must be > 0.
 * @returns The number of units of @p type the map should hold (>= 1).
 */
[[nodiscard]] std::size_t targetQuantity(world::ResourceType type, int width,
                                         int height);

}  // namespace zappy::loot
