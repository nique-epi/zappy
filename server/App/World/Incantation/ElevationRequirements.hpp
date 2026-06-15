/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ElevationRequirements — per-level player and mineral prerequisites
*/

#pragma once

#include <array>
#include <cstddef>
#include "App/World/Resources/ResourceType.hpp"

namespace zappy::world {

/**
 * @brief Prerequisites a group must gather on one tile to climb one level.
 *
 * The mineral costs are indexed by @ref ResourceType, so @c stones[Linemate]
 * is the linemate count required. The Food slot is always zero: food never
 * takes part in an elevation.
 */
struct ElevationRequirement {
  /// Players of the elevating level that must stand on the tile, initiator
  /// included.
  std::size_t playersRequired;
  /// Minerals consumed from the tile on success, indexed by ResourceType.
  std::array<std::size_t, resourceTypeCount> stones;
};

/// Lowest level that can start an elevation (1 -> 2).
constexpr int minElevationLevel = 1;
/// Highest level that can start an elevation (7 -> 8).
constexpr int maxElevationLevel = 7;

/**
 * @brief Prerequisites for elevating a player from @p fromLevel to the next.
 *
 * Values come straight from the subject's elevation table (1->2 … 7->8).
 *
 * @param[in] fromLevel Current level of the elevating players, in
 *                      `[minElevationLevel, maxElevationLevel]`.
 * @returns The player count and mineral costs for that step.
 * @throws InvalidElevationLevelException if @p fromLevel is outside the
 *         elevable range.
 */
[[nodiscard]] const ElevationRequirement& elevationRequirement(int fromLevel);

}  // namespace zappy::world
