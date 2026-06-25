/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** MenuCamera
*/

#include "Menu/MenuCamera.hpp"
#include <raylib.h>
#include <raymath.h>
#include <cmath>
#include "Menu/MenuConfig.hpp"

namespace zappy::gui {

namespace cfg = menu::config;

MenuCamera::MenuCamera(Vector3 target)
    : target_(target),
      yaw_(0.0F),
      distance_(cfg::cameraDistance),
      pitch_(cfg::cameraPitchDegrees * DEG2RAD) {
  camera_.target = target_;
  camera_.up = Vector3{0.0F, 1.0F, 0.0F};
  camera_.fovy = cfg::cameraFieldOfViewDegrees;
  camera_.projection = CAMERA_PERSPECTIVE;
  update(0.0F);
}

void MenuCamera::update(float deltaTime) {
  yaw_ += cfg::cameraYawSpeedDegrees * DEG2RAD * deltaTime;

  const float horizontalRadius = distance_ * std::cos(pitch_);
  camera_.position = Vector3{target_.x + horizontalRadius * std::sin(yaw_),
                             target_.y + distance_ * std::sin(pitch_),
                             target_.z + horizontalRadius * std::cos(yaw_)};
  camera_.target = target_;
}

const Camera3D& MenuCamera::camera() const { return camera_; }

}  // namespace zappy::gui
