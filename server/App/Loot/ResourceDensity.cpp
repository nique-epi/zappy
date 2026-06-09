/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ResourceDensity — per-kind spawn density and target stock on the world floor
*/

#include "App/Loot/ResourceDensity.hpp"
#include <algorithm>
#include <array>
#include "App/World/Exceptions/WorldException.hpp"

namespace zappy::loot {

namespace {
constexpr std::size_t densityDenominator = 100;
constexpr std::array<std::size_t, world::resourceTypeCount> densityNumerator = {
    50, 30, 15, 10, 10, 8, 5};
}  // namespace

std::size_t targetQuantity(world::ResourceType type, int width, int height) {
  if (width <= 0 || height <= 0) {
    throw world::InvalidMapDimensionsException(width, height);
  }

  const std::size_t tiles =
      static_cast<std::size_t>(width) * static_cast<std::size_t>(height);
  const std::size_t numerator =
      densityNumerator[static_cast<std::size_t>(type)];
  const std::size_t quantity = tiles * numerator / densityDenominator;

  return std::max<std::size_t>(1, quantity);
}

}  // namespace zappy::loot
