/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** WorldCamera
*/

#pragma once  // NOLINT(llvm-header-guard)

#include <raylib.h>
#include "Bindings/KeyBindings.hpp"

namespace zappy::gui {

/**
 * @brief Free-flying 3D camera looking down on the world, controlled by
 *        keyboard movement, mouse-wheel zoom, and right-click rotation.
 */
class WorldCamera {
 public:
  explicit WorldCamera(Vector3 target);

  /**
   * @brief Reads input and updates the underlying camera state.
   *
   * @param[in] deltaTime Time elapsed since the last frame, in seconds.
   */
  void update(float deltaTime, const KeyBindings& bindings);

  [[nodiscard]] const Camera3D& camera() const;

 private:
  void applyMovement(float deltaTime, const KeyBindings& bindings);
  void applyZoom();
  void applyRotation();

  Camera3D camera_{};
  float pitch_;
};

}  // namespace zappy::gui
