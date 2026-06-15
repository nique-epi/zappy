/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Game world exception hierarchy
*/

#pragma once

#include <stdexcept>
#include <string>

namespace zappy::world {

/**
 * @brief Root exception for every game-world error.
 *
 * The world layer never lets a standard-library exception escape: any invalid
 * construction or operation on the map and its tiles surfaces as a subclass of
 * @ref WorldException so callers can catch the whole family at once.
 */
class WorldException : public std::runtime_error {
 public:
  explicit WorldException(const std::string& message);
};

/**
 * @brief Thrown when a map is built with a non strictly positive dimension.
 */
class InvalidMapDimensionsException : public WorldException {
 public:
  InvalidMapDimensionsException(int width, int height);
};

/**
 * @brief Thrown when a map with valid dimensions cannot be allocated (the world
 *        is too large for the available memory). Translates the
 * `std::bad_alloc` / `std::length_error` raised by the tile storage so no
 * standard exception escapes the world layer.
 */
class MapAllocationException : public WorldException {
 public:
  MapAllocationException(int width, int height);
};

/**
 * @brief Thrown when an operation references a team name that was not
 *        declared on the command line.
 */
class UnknownTeamException : public WorldException {
 public:
  explicit UnknownTeamException(const std::string& teamName);
};

/**
 * @brief Thrown when an AI connection or a hatching request targets a team
 *        whose slots are all already in use.
 */
class NoFreeSlotException : public WorldException {
 public:
  explicit NoFreeSlotException(const std::string& teamName);
};

/**
 * @brief Thrown when an elevation requirement is requested for a level that
 *        cannot start an elevation (outside `[1, 7]`).
 */
class InvalidElevationLevelException : public WorldException {
 public:
  explicit InvalidElevationLevelException(int fromLevel);
};

}  // namespace zappy::world
