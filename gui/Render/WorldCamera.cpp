/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** WorldCamera
*/

#include "Render/WorldCamera.hpp"
#include <raylib.h>
#include <raymath.h>
#include <rcamera.h>
#include <algorithm>
#include <cmath>
#include "Bindings/KeyBindings.hpp"
#include "Render/WindowConfig.hpp"

namespace zappy::gui {

namespace cfg = config;

namespace {

Vector3 cameraOffset(
    float distance,  // NOLINT(bugprone-easily-swappable-parameters)
    float pitchRadians) {
  return Vector3{0.0F, std::sin(pitchRadians) * distance,
                 std::cos(pitchRadians) * distance};
}

}  // namespace

WorldCamera::WorldCamera(Vector3 target)
    : pitch_(cfg::CAMERA_INITIAL_PITCH_DEGREES * DEG2RAD) {
  camera_.target = target;
  camera_.up = Vector3{0.0F, 1.0F, 0.0F};
  camera_.fovy = cfg::CAMERA_FOV_Y_DEGREES;
  camera_.projection = CAMERA_PERSPECTIVE;
  camera_.position =
      Vector3Add(target, cameraOffset(cfg::CAMERA_INITIAL_DISTANCE, pitch_));
}

void WorldCamera::update(float deltaTime, const KeyBindings& bindings) {
  applyMovement(deltaTime, bindings);
  applyZoom();
  applyRotation();
}

const Camera3D& WorldCamera::camera() const { return camera_; }

void WorldCamera::applyMovement(float deltaTime, const KeyBindings& bindings) {
  const float distance = cfg::CAMERA_MOVE_SPEED * deltaTime;
  if (IsKeyDown(bindings.cameraForward) || IsKeyDown(KEY_UP)) {
    CameraMoveForward(&camera_, distance, true);
  }
  if (IsKeyDown(bindings.cameraBackward) || IsKeyDown(KEY_DOWN)) {
    CameraMoveForward(&camera_, -distance, true);
  }
  if (IsKeyDown(bindings.cameraLeft) || IsKeyDown(KEY_LEFT)) {
    CameraMoveRight(&camera_, -distance, true);
  }
  if (IsKeyDown(bindings.cameraRight) || IsKeyDown(KEY_RIGHT)) {
    CameraMoveRight(&camera_, distance, true);
  }
}

void WorldCamera::applyZoom() {
  const float wheelMove = GetMouseWheelMove();
  if (wheelMove == 0.0F) {
    return;
  }
  const float currentDistance =
      Vector3Distance(camera_.position, camera_.target);
  const float targetDistance =  // NOLINT(cppcoreguidelines-init-variables)
      std::clamp(currentDistance - (wheelMove * cfg::CAMERA_ZOOM_SPEED),
                 cfg::CAMERA_MIN_DISTANCE, cfg::CAMERA_MAX_DISTANCE);
  CameraMoveToTarget(&camera_, targetDistance - currentDistance);
}

void WorldCamera::applyRotation() {
  if (!IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
    return;
  }
  const Vector2 mouseDelta = GetMouseDelta();
  CameraYaw(&camera_, -mouseDelta.x * cfg::CAMERA_ROTATION_SPEED, true);

  const float minPitch = cfg::CAMERA_MIN_PITCH_DEGREES * DEG2RAD;
  const float maxPitch = cfg::CAMERA_MAX_PITCH_DEGREES * DEG2RAD;
  const float targetPitch =  // NOLINT(cppcoreguidelines-init-variables)
      std::clamp(pitch_ - (mouseDelta.y * cfg::CAMERA_ROTATION_SPEED), minPitch,
                 maxPitch);
  CameraPitch(&camera_, targetPitch - pitch_, true, true, false);
  pitch_ = targetPitch;
}

}  // namespace zappy::gui
