/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** IncantationRendererConfig
*/

#pragma once

namespace zappy::gui::config {

constexpr int INCANTATION_FRAME_COUNT = 60;
constexpr float INCANTATION_ANIM_FPS = 12.0F;
constexpr float INCANTATION_CIRCLE_SCALE = 2.0F;
constexpr float INCANTATION_GROUND_Y = 0.5F;
constexpr unsigned char INCANTATION_ACTIVE_ALPHA = 220;
constexpr float INCANTATION_FLASH_DURATION = 3.0F;
constexpr float INCANTATION_ROTATION_DEG = 1000.0F;
constexpr const char* INCANTATION_LOAD_ERROR =
    "[IncantationRenderer] ERROR: failed to load ";

}  // namespace zappy::gui::config
