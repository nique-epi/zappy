/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for the SpeedControl frequency clamp policy
*/

#include <gtest/gtest.h>
#include "Render/SpeedControl.hpp"
#include "Render/WindowConfig.hpp"

namespace cfg = zappy::gui::config;
using zappy::gui::SpeedControl;

// Given a frequency already inside the GUI range
// When clampFrequency is applied
// Then the value is returned unchanged
TEST(SpeedControl, ClampKeepsInRangeValue) {
  EXPECT_EQ(SpeedControl::clampFrequency(100), 100);
}

// Given a frequency below the GUI minimum
// When clampFrequency is applied
// Then the result is raised to SPEED_MIN
TEST(SpeedControl, ClampRaisesBelowMinToMin) {
  EXPECT_EQ(SpeedControl::clampFrequency(0), cfg::SPEED_MIN);
  EXPECT_EQ(SpeedControl::clampFrequency(-50), cfg::SPEED_MIN);
}

// Given a frequency above the GUI maximum
// When clampFrequency is applied
// Then the result is lowered to SPEED_MAX
TEST(SpeedControl, ClampLowersAboveMaxToMax) {
  EXPECT_EQ(SpeedControl::clampFrequency(cfg::SPEED_MAX + 500), cfg::SPEED_MAX);
}

// Given a mid-range frequency
// When increaseFrequency is applied
// Then the result grows by exactly one step
TEST(SpeedControl, IncreaseAddsOneStep) {
  EXPECT_EQ(SpeedControl::increaseFrequency(100), 100 + cfg::SPEED_STEP);
}

// Given a mid-range frequency
// When decreaseFrequency is applied
// Then the result shrinks by exactly one step
TEST(SpeedControl, DecreaseSubtractsOneStep) {
  EXPECT_EQ(SpeedControl::decreaseFrequency(100), 100 - cfg::SPEED_STEP);
}

// Given a frequency at the GUI maximum
// When increaseFrequency is applied
// Then the result stays clamped at SPEED_MAX
TEST(SpeedControl, IncreaseClampsAtMax) {
  EXPECT_EQ(SpeedControl::increaseFrequency(cfg::SPEED_MAX), cfg::SPEED_MAX);
}

// Given a frequency at the GUI minimum
// When decreaseFrequency is applied
// Then the result stays clamped at SPEED_MIN
TEST(SpeedControl, DecreaseClampsAtMin) {
  EXPECT_EQ(SpeedControl::decreaseFrequency(cfg::SPEED_MIN), cfg::SPEED_MIN);
}

// Given a frequency one step short of the maximum
// When increaseFrequency would overshoot
// Then the result is clamped to SPEED_MAX rather than exceeding it
TEST(SpeedControl, IncreaseNearMaxDoesNotOvershoot) {
  EXPECT_EQ(SpeedControl::increaseFrequency(cfg::SPEED_MAX - 1),
            cfg::SPEED_MAX);
}