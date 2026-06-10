/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Game world exception hierarchy
*/

#include "App/World/Exceptions/WorldException.hpp"
#include <string>
#include "Error/Messages/TeamMessages.hpp"
#include "Error/Messages/WorldMessages.hpp"

namespace zappy::world {

WorldException::WorldException(const std::string& message)
    : std::runtime_error(message) {}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
InvalidMapDimensionsException::InvalidMapDimensionsException(int width,
                                                             int height)
    : WorldException(error::messages::INVALID_MAP_DIMENSIONS_PREFIX +
                     std::to_string(width) + "x" + std::to_string(height) +
                     error::messages::INVALID_MAP_DIMENSIONS_SUFFIX) {}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
MapAllocationException::MapAllocationException(int width, int height)
    : WorldException(error::messages::MAP_ALLOCATION_FAILED_PREFIX +
                     std::to_string(width) + "x" + std::to_string(height) +
                     error::messages::MAP_ALLOCATION_FAILED_SUFFIX) {}

UnknownTeamException::UnknownTeamException(const std::string& teamName)
    : WorldException(error::messages::UNKNOWN_TEAM_PREFIX + teamName +
                     error::messages::UNKNOWN_TEAM_SUFFIX) {}

NoFreeSlotException::NoFreeSlotException(const std::string& teamName)
    : WorldException(error::messages::NO_FREE_SLOT_PREFIX + teamName +
                     error::messages::NO_FREE_SLOT_SUFFIX) {}

}  // namespace zappy::world
