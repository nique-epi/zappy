/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** MenuCamera — auto-rotating camera for the menu background simulation
*/

#pragma once

#include <raylib.h>

namespace zappy::gui {

/**
 * @brief Orbital camera that auto-rotates around a fixed target with no user
 * input.
 */
class MenuCamera {
 public:
  explicit MenuCamera(Vector3 target);

  /**
   * @brief Advances the yaw angle and recomputes the camera position.
   *
   * @param[in] deltaTime Elapsed time in seconds since the last frame.
   */
  void update(float deltaTime);

  [[nodiscard]] const Camera3D& camera() const;

 private:
  Camera3D camera_{};
  Vector3 target_{};
  float yaw_{};
  float distance_{};
  float pitch_{};
};

}  // namespace zappy::gui
