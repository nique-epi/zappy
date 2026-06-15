/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** TimeUnit — live reciprocal time unit `f` shared by the action pipeline
*/

#include "App/TimeUnit.hpp"
#include "App/Scheduler/Exceptions/SchedulerException.hpp"

namespace zappy::server {

TimeUnit::TimeUnit(int frequency) : frequency_(frequency) {
  if (frequency <= 0) {
    throw InvalidFrequencyException(frequency);
  }
}

int TimeUnit::value() const { return frequency_; }

void TimeUnit::set(int frequency) {
  if (frequency <= 0) {
    throw InvalidFrequencyException(frequency);
  }
  frequency_ = frequency;
}

}  // namespace zappy::server
