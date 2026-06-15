/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ElevationRequirements — per-level player and mineral prerequisites
*/

#include "App/World/Incantation/ElevationRequirements.hpp"
#include <array>
#include "App/World/Exceptions/WorldException.hpp"

namespace zappy::world {

namespace {

constexpr std::size_t elevationStepCount = 7;

// Indexed by ResourceType: {Food, Linemate, Deraumere, Sibur, Mendiane,
// Phiras, Thystame}. Food is always zero — it never takes part in a ritual.
constexpr std::array<ElevationRequirement, elevationStepCount>
    elevationRequirementTable{{
        {.playersRequired = 1, .stones = {0, 1, 0, 0, 0, 0, 0}},
        {.playersRequired = 2, .stones = {0, 1, 1, 1, 0, 0, 0}},
        {.playersRequired = 2, .stones = {0, 2, 0, 1, 0, 2, 0}},
        {.playersRequired = 4, .stones = {0, 1, 1, 2, 0, 1, 0}},
        {.playersRequired = 4, .stones = {0, 1, 2, 1, 3, 0, 0}},
        {.playersRequired = 6, .stones = {0, 1, 2, 3, 0, 1, 0}},
        {.playersRequired = 6, .stones = {0, 2, 2, 2, 2, 2, 1}},
    }};

}  // namespace

const ElevationRequirement& elevationRequirement(int fromLevel) {
  if (fromLevel < minElevationLevel || fromLevel > maxElevationLevel) {
    throw InvalidElevationLevelException(fromLevel);
  }
  return elevationRequirementTable[static_cast<std::size_t>(fromLevel -
                                                            minElevationLevel)];
}

}  // namespace zappy::world
