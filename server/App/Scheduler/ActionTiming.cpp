/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ActionTiming — converts a game action cost into a real-time delay
*/

#include "App/Scheduler/ActionTiming.hpp"
#include "App/Scheduler/Exceptions/SchedulerException.hpp"

namespace zappy::server {

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
std::chrono::nanoseconds actionDuration(int timeUnits, int frequency) {
  if (frequency <= 0) {
    throw InvalidFrequencyException(frequency);
  }
  const std::chrono::duration<double> seconds(static_cast<double>(timeUnits) /
                                              static_cast<double>(frequency));
  return std::chrono::round<std::chrono::nanoseconds>(seconds);
}

}  // namespace zappy::server
