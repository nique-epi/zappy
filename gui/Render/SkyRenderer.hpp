/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** SkyRenderer — equirectangular HDR skybox (sphere seen from inside)
*/

#pragma once  // NOLINT(llvm-header-guard)

#include <raylib.h>

namespace zappy::gui {

class SkyRenderer {
 public:
  static void load();
  static void unload();

  /**
   * @brief Draws the sky sphere centered on the camera.
   *
   * Must be called inside BeginMode3D / EndMode3D, before any other geometry.
   */
  static void draw(Camera3D camera);
};

}  // namespace zappy::gui
