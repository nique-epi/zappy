/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Scheduler — deadline-ordered queue of timed game events
*/

#include "App/Scheduler/Scheduler.hpp"
#include <algorithm>
#include <limits>
#include <utility>
#include <vector>

namespace zappy::server {

void Scheduler::scheduleAt(TimePoint deadline, Callback action) {
  events_.emplace(deadline, std::move(action));
}

bool Scheduler::empty() const { return events_.empty(); }

std::size_t Scheduler::pendingCount() const { return events_.size(); }

int Scheduler::timeoutUntilNext(TimePoint now) const {
  if (events_.empty()) {
    return blockIndefinitely;
  }
  const TimePoint next = events_.begin()->first;
  if (next <= now) {
    return 0;
  }
  const auto milliseconds =
      std::chrono::ceil<std::chrono::milliseconds>(next - now).count();
  return static_cast<int>(std::min<std::chrono::milliseconds::rep>(
      milliseconds, std::numeric_limits<int>::max()));
}

void Scheduler::runDue(TimePoint now) {
  std::vector<Callback> due;

  while (!events_.empty() && events_.begin()->first <= now) {
    due.push_back(std::move(events_.begin()->second));
    events_.erase(events_.begin());
  }
  for (const Callback &action : due) {
    action();
  }
}

}  // namespace zappy::server
