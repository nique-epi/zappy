/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for the deadline-ordered Scheduler
*/

#include <gtest/gtest.h>
#include <chrono>
#include <vector>
#include "App/Scheduler/Scheduler.hpp"

using namespace std::chrono_literals;
using zappy::server::blockIndefinitely;
using zappy::server::Scheduler;

namespace {

Scheduler::TimePoint base() { return Scheduler::Clock::time_point{}; }

}  // namespace

TEST(Scheduler, EmptyQueueBlocksIndefinitely) {
  const Scheduler scheduler;

  EXPECT_TRUE(scheduler.empty());
  EXPECT_EQ(scheduler.pendingCount(), 0U);
  EXPECT_EQ(scheduler.timeoutUntilNext(base()), blockIndefinitely);
}

TEST(Scheduler, TimeoutIsTheDelayUntilTheEarliestDeadline) {
  Scheduler scheduler;
  scheduler.scheduleAt(base() + 100ms, [] {});

  EXPECT_EQ(scheduler.timeoutUntilNext(base()), 100);
  EXPECT_EQ(scheduler.pendingCount(), 1U);
}

TEST(Scheduler, TimeoutIsZeroWhenAnEventIsAlreadyDue) {
  Scheduler scheduler;
  scheduler.scheduleAt(base() + 50ms, [] {});

  EXPECT_EQ(scheduler.timeoutUntilNext(base() + 50ms), 0);
  EXPECT_EQ(scheduler.timeoutUntilNext(base() + 80ms), 0);
}

TEST(Scheduler, TimeoutTracksTheEarliestOfSeveralDeadlines) {
  Scheduler scheduler;
  scheduler.scheduleAt(base() + 300ms, [] {});
  scheduler.scheduleAt(base() + 70ms, [] {});
  scheduler.scheduleAt(base() + 150ms, [] {});

  EXPECT_EQ(scheduler.timeoutUntilNext(base()), 70);
}

TEST(Scheduler, RunDueExecutesOnlyPassedDeadlines) {
  Scheduler scheduler;
  bool early = false;
  bool late = false;
  scheduler.scheduleAt(base() + 10ms, [&] { early = true; });
  scheduler.scheduleAt(base() + 500ms, [&] { late = true; });

  scheduler.runDue(base() + 20ms);

  EXPECT_TRUE(early);
  EXPECT_FALSE(late);
  EXPECT_EQ(scheduler.pendingCount(), 1U);
}

TEST(Scheduler, RunDueFiresEarliestFirst) {
  Scheduler scheduler;
  std::vector<int> order;
  scheduler.scheduleAt(base() + 30ms, [&] { order.push_back(3); });
  scheduler.scheduleAt(base() + 10ms, [&] { order.push_back(1); });
  scheduler.scheduleAt(base() + 20ms, [&] { order.push_back(2); });

  scheduler.runDue(base() + 100ms);

  EXPECT_EQ(order, (std::vector<int>{1, 2, 3}));
  EXPECT_TRUE(scheduler.empty());
}

TEST(Scheduler, RunDueDoesNothingWhenNothingIsDue) {
  Scheduler scheduler;
  bool fired = false;
  scheduler.scheduleAt(base() + 100ms, [&] { fired = true; });

  scheduler.runDue(base());

  EXPECT_FALSE(fired);
  EXPECT_EQ(scheduler.pendingCount(), 1U);
}

TEST(Scheduler, CallbackMayScheduleAFollowUpEvent) {
  Scheduler scheduler;
  int fired = 0;
  scheduler.scheduleAt(base() + 10ms, [&] {
    fired++;
    scheduler.scheduleAt(base() + 200ms, [&] { fired++; });
  });

  scheduler.runDue(base() + 20ms);
  EXPECT_EQ(fired, 1);

  scheduler.runDue(base() + 200ms);
  EXPECT_EQ(fired, 2);
}
