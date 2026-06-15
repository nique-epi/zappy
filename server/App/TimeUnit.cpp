/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** TimeUnit — live reciprocal time unit `f` shared by the action pipeline
*/

#include "App/TimeUnit.hpp"
#include "App/Scheduler/Exceptions/SchedulerException.hpp"

namespace zappy::server {

namespace {

int validateFrequency(int frequency) {
  if (frequency <= 0) {
    throw InvalidFrequencyException(frequency);
  }
  return frequency;
}

}  // namespace

TimeUnit::TimeUnit(int frequency) : frequency_(validateFrequency(frequency)) {}

int TimeUnit::value() const { return frequency_; }

void TimeUnit::set(int frequency) { frequency_ = validateFrequency(frequency); }

}  // namespace zappy::server
