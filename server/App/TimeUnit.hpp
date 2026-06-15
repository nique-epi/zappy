/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** TimeUnit — live reciprocal time unit `f` shared by the action pipeline
*/

#pragma once

namespace zappy::server {

/**
 * @brief Mutable holder for the reciprocal time unit `f`.
 *
 * The frequency divides every action cost (`cost / f` seconds), so the AI
 * action pipeline reads it at scheduling time rather than capturing a fixed
 * value. The GUI `sst` command mutates it live; `sgt` reads it. A single
 * instance is shared by reference, never copied.
 */
class TimeUnit {
 public:
  /**
   * @brief Build the holder with the startup frequency.
   * @param[in] frequency Reciprocal time unit `f`; must be strictly positive.
   * @throws InvalidFrequencyException if @p frequency is not strictly positive.
   */
  explicit TimeUnit(int frequency);

  /// Current reciprocal time unit `f`.
  [[nodiscard]] int value() const;

  /**
   * @brief Replace the reciprocal time unit, effective for future actions.
   * @param[in] frequency New `f`; must be strictly positive.
   * @throws InvalidFrequencyException if @p frequency is not strictly positive.
   */
  void set(int frequency);

 private:
  int frequency_;
};

}  // namespace zappy::server
