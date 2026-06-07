/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ActionTiming — converts a game action cost into a real-time delay
*/

#pragma once

#include <chrono>

namespace zappy::server {

/**
 * @brief Real-time delay of an action costing @p timeUnits game time units at
 *        frequency @p frequency.
 *
 * Applies the subject rule: an action of N time units lasts `N / f` seconds
 * (e.g. Forward/Take/Set = 7/f, Inventory = 1/f, Fork = 42/f, Incantation =
 * 300/f). A zero-cost action (Connect_nbr) yields a zero delay and is therefore
 * due immediately. The returned delay is meant to be added to a
 * Scheduler::TimePoint to obtain the action's deadline.
 *
 * @param[in] timeUnits Action cost in game time units (>= 0).
 * @param[in] frequency Reciprocal time unit `f` (> 0).
 * @returns The wall-clock delay to wait before the action executes.
 * @throws InvalidActionCostException if @p timeUnits is negative.
 * @throws InvalidFrequencyException if @p frequency is not strictly positive.
 */
std::chrono::nanoseconds actionDuration(int timeUnits, int frequency);

}  // namespace zappy::server
