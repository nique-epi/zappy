/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Scheduler — deadline-ordered queue of timed game events
*/

#pragma once

#include <chrono>
#include <cstddef>
#include <functional>
#include <map>

namespace zappy::server {

/// poll() timeout meaning "block until socket activity" (no event pending).
constexpr int blockIndefinitely = -1;

/**
 * @brief A queue of callbacks each due at a wall-clock deadline, ordered by
 *        deadline.
 *
 * This is the timing engine behind the no-active-waiting requirement: the poll
 * loop asks @ref timeoutUntilNext how long it may sleep, blocks in poll() until
 * either a socket wakes it or that delay elapses, then calls @ref runDue to
 * execute whatever became due. The scheduler never reads the clock itself — the
 * caller passes the current time — so its behaviour is fully deterministic and
 * testable without sleeping.
 */
class Scheduler {
 public:
  using Clock = std::chrono::steady_clock;
  using TimePoint = Clock::time_point;
  using Callback = std::function<void()>;

  /**
   * @brief Register @p action to run once @p deadline has passed.
   * @param[in] deadline Absolute time at which @p action becomes due.
   * @param[in] action   Callback executed by @ref runDue.
   */
  void scheduleAt(TimePoint deadline, Callback action);

  /// Whether no event is currently pending.
  [[nodiscard]] bool empty() const;

  /// Number of events still waiting to run.
  [[nodiscard]] std::size_t pendingCount() const;

  /**
   * @brief Milliseconds the poll loop may block before the next event is due.
   * @param[in] now The current time reference.
   * @returns 0 if an event is already due, the (rounded-up) delay to the
   *          earliest deadline otherwise, or @ref blockIndefinitely (-1) when
   *          no event is pending — the loop then blocks until socket activity.
   */
  [[nodiscard]] int timeoutUntilNext(TimePoint now) const;

  /**
   * @brief Run every action due at or before @p now, earliest first.
   *
   * Each action is removed from the queue before being invoked, so a callback
   * may safely schedule further events.
   *
   * @param[in] now The current time reference.
   */
  void runDue(TimePoint now);

 private:
  std::multimap<TimePoint, Callback> events_;
};

}  // namespace zappy::server
