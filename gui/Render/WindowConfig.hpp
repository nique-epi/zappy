/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** WindowConfig
*/

#pragma once

namespace zappy::gui::config {

constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 720;
constexpr int TARGET_FPS = 60;
constexpr const char* WINDOW_TITLE = "Zappy GUI";

constexpr int MARGIN_X = 20;
constexpr int TITLE_Y = 20;
constexpr int TITLE_FONT_SIZE = 30;
constexpr int HUD_Y = 60;
constexpr int HUD_FONT_SIZE = 20;

constexpr float TILE_SIZE = 1.0F;

constexpr float CAMERA_FOV_Y_DEGREES = 45.0F;
constexpr float CAMERA_INITIAL_DISTANCE = 20.0F;
constexpr float CAMERA_MIN_DISTANCE = 5.0F;
constexpr float CAMERA_MAX_DISTANCE = 60.0F;
constexpr float CAMERA_INITIAL_PITCH_DEGREES = 60.0F;
constexpr float CAMERA_MIN_PITCH_DEGREES = 5.0F;
constexpr float CAMERA_MAX_PITCH_DEGREES = 85.0F;
constexpr float CAMERA_MOVE_SPEED = 15.0F;
constexpr float CAMERA_ZOOM_SPEED = 2.0F;
constexpr float CAMERA_ROTATION_SPEED = 0.003F;

}  // namespace zappy::gui::config
