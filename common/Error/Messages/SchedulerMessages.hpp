/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** SchedulerMessages — user-facing strings emitted by the action scheduler
*/

#pragma once

namespace zappy::error::messages {

inline constexpr const char *INVALID_FREQUENCY_PREFIX = "invalid frequency ";
inline constexpr const char *INVALID_FREQUENCY_SUFFIX =
    ": f must be strictly positive";

inline constexpr const char *INVALID_ACTION_COST_PREFIX =
    "invalid action cost ";
inline constexpr const char *INVALID_ACTION_COST_SUFFIX =
    ": time units must be non-negative";

}  // namespace zappy::error::messages
