/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for the action cost / frequency timing rule
*/

#include <gtest/gtest.h>
#include <chrono>
#include "App/Scheduler/ActionTiming.hpp"
#include "App/Scheduler/Exceptions/SchedulerException.hpp"

using zappy::server::actionDuration;
using zappy::server::InvalidFrequencyException;
using zappy::server::SchedulerException;

namespace {

long long milliseconds(int timeUnits, int frequency) {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             actionDuration(timeUnits, frequency))
      .count();
}

}  // namespace

TEST(ActionTiming, AppliesTheSubjectDurationsAtDefaultFrequency) {
  EXPECT_EQ(milliseconds(7, 100), 70);
  EXPECT_EQ(milliseconds(1, 100), 10);
  EXPECT_EQ(milliseconds(42, 100), 420);
  EXPECT_EQ(milliseconds(300, 100), 3000);
}

TEST(ActionTiming, ZeroCostActionIsImmediate) {
  EXPECT_EQ(milliseconds(0, 100), 0);
}

TEST(ActionTiming, FasterFrequencyShortensTheDelay) {
  EXPECT_EQ(milliseconds(7, 70), 100);
  EXPECT_EQ(milliseconds(7, 700), 10);
}

TEST(ActionTiming, RejectsNonPositiveFrequency) {
  EXPECT_THROW(actionDuration(7, 0), InvalidFrequencyException);
  EXPECT_THROW(actionDuration(7, -100), InvalidFrequencyException);
}

TEST(ActionTiming, InvalidFrequencyIsCatchableAsSchedulerException) {
  EXPECT_THROW(actionDuration(7, 0), SchedulerException);
}
