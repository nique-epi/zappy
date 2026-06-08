/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** WorldMessages — user-facing strings emitted by the game world
*/

#pragma once

namespace zappy::error::messages {

inline constexpr const char *INVALID_MAP_DIMENSIONS_PREFIX =
    "invalid map dimensions ";
inline constexpr const char *INVALID_MAP_DIMENSIONS_SUFFIX =
    ": width and height must be strictly positive";

inline constexpr const char *MAP_ALLOCATION_FAILED_PREFIX =
    "failed to allocate map ";
inline constexpr const char *MAP_ALLOCATION_FAILED_SUFFIX =
    ": world too large to allocate";

}  // namespace zappy::error::messages
