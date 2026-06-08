/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Scheduler exception hierarchy
*/

#pragma once

#include <stdexcept>
#include <string>

namespace zappy::server {

/**
 * @brief Root exception for every scheduling / timing error.
 *
 * Any invalid timing parameter surfaces as a subclass of @ref
 * SchedulerException so callers can catch the whole family at once.
 */
class SchedulerException : public std::runtime_error {
 public:
  explicit SchedulerException(const std::string &message);
};

/**
 * @brief Thrown when the frequency `f` used to time actions is not strictly
 *        positive (it divides the action cost, so it must be >= 1).
 */
class InvalidFrequencyException : public SchedulerException {
 public:
  explicit InvalidFrequencyException(int frequency);
};

/**
 * @brief Thrown when an action cost (in game time units) is negative; a cost
 *        must be >= 0.
 */
class InvalidActionCostException : public SchedulerException {
 public:
  explicit InvalidActionCostException(int timeUnits);
};

}  // namespace zappy::server
