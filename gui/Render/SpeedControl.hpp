/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** SpeedControl — Raygui overlay to tune the server time unit (sst/sgt)
*/

#pragma once

#include <algorithm>
#include "Render/WindowConfig.hpp"

namespace zappy::gui {

class CommandSender;

/**
 * @brief Spectator-facing overlay to speed up or slow down the simulation.
 *
 * Renders a small Raygui panel in a window corner with @c - / @c + buttons
 * around the current reciprocal time unit @c f. Each button click sends a
 * clamped @c sst command through a @ref CommandSender; the displayed value
 * stays server-authoritative (the caller passes the latest @c f, refreshed
 * by every @c sgt / @c sst reply), so the GUI never drifts from the server.
 *
 * The clamp policy (@ref clampFrequency, @ref increaseFrequency,
 * @ref decreaseFrequency) is pure and @c constexpr so the GUI-side min/max
 * bounds are unit-testable without a render context.
 */
class SpeedControl {
 public:
  explicit SpeedControl(CommandSender& sender);
  ~SpeedControl() = default;

  SpeedControl(const SpeedControl&) = delete;
  SpeedControl& operator=(const SpeedControl&) = delete;
  SpeedControl(SpeedControl&&) = delete;
  SpeedControl& operator=(SpeedControl&&) = delete;

  /**
   * @brief Draw the overlay and send an @c sst command on button interaction.
   *
   * @param[in] currentFrequency Latest reciprocal time unit @c f known from
   *                             the server, displayed as-is.
   */
  void draw(int currentFrequency);

  /**
   * @brief Clamp @p frequency to the GUI-accepted range [SPEED_MIN, SPEED_MAX].
   */
  [[nodiscard]] static constexpr int clampFrequency(int frequency) {
    return std::clamp(frequency, config::SPEED_MIN, config::SPEED_MAX);
  }

  /**
   * @brief Next faster frequency: @p frequency raised by one step, clamped.
   */
  [[nodiscard]] static constexpr int increaseFrequency(int frequency) {
    return clampFrequency(frequency + config::SPEED_STEP);
  }

  /**
   * @brief Next slower frequency: @p frequency lowered by one step, clamped.
   */
  [[nodiscard]] static constexpr int decreaseFrequency(int frequency) {
    return clampFrequency(frequency - config::SPEED_STEP);
  }

 private:
  CommandSender& sender_;
};

}  // namespace zappy::gui
