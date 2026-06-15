/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for the TimeUnit holder
*/

#include <gtest/gtest.h>
#include "App/Scheduler/Exceptions/SchedulerException.hpp"
#include "App/TimeUnit.hpp"

using zappy::server::InvalidFrequencyException;
using zappy::server::TimeUnit;

TEST(TimeUnit, ExposesTheStartupFrequency) {
  /*
   * Given a TimeUnit built with frequency 100
   * When its value is read
   * Then it returns 100
   */
  const TimeUnit timeUnit(100);

  EXPECT_EQ(timeUnit.value(), 100);
}

TEST(TimeUnit, SetReplacesTheFrequency) {
  /*
   * Given a TimeUnit built with frequency 100
   * When it is set to 250
   * Then its value becomes 250
   */
  TimeUnit timeUnit(100);

  timeUnit.set(250);

  EXPECT_EQ(timeUnit.value(), 250);
}

TEST(TimeUnit, ConstructionRejectsNonPositiveFrequency) {
  /*
   * Given the rule that `f` must stay strictly positive
   * When a TimeUnit is built with zero or a negative frequency
   * Then it throws InvalidFrequencyException
   */
  EXPECT_THROW(TimeUnit(0), InvalidFrequencyException);
  EXPECT_THROW(TimeUnit(-1), InvalidFrequencyException);
}

TEST(TimeUnit, SetRejectsNonPositiveFrequencyAndKeepsTheOldValue) {
  /*
   * Given a TimeUnit built with frequency 100
   * When it is set to a non-positive value
   * Then it throws and the previous value is preserved
   */
  TimeUnit timeUnit(100);

  EXPECT_THROW(timeUnit.set(0), InvalidFrequencyException);
  EXPECT_EQ(timeUnit.value(), 100);
}
