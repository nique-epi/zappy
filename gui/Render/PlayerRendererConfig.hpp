/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** PlayerRendererConfig
*/

#pragma once

#include <raylib.h>
#include <array>

namespace zappy::gui {

struct DirectionVec {
  float x;
  float z;
};

}  // namespace zappy::gui

namespace zappy::gui::config {

constexpr float PLAYER_RADIUS = 0.18F;
constexpr float PLAYER_HEIGHT = 0.45F;
constexpr float PLAYER_BASE_Y = 0.0F;
constexpr float PLAYER_CENTER_Y = PLAYER_BASE_Y + (PLAYER_HEIGHT / 2.0F);

constexpr float PLAYER_ARROW_RADIUS = 0.07F;
constexpr float PLAYER_ARROW_HEIGHT = 0.22F;
constexpr float PLAYER_ARROW_OFFSET = PLAYER_RADIUS + 0.05F;

constexpr float PLAYER_LABEL_Y = PLAYER_BASE_Y + PLAYER_HEIGHT + 0.3F;

constexpr int PLAYER_LABEL_FONT_SIZE = 18;
constexpr int PLAYER_LABEL_SHADOW_OFFSET = 1;
constexpr int PLAYER_CYLINDER_SIDES = 12;
constexpr int PLAYER_ARROW_SIDES = 6;

constexpr float PLAYER_WIRE_DARKNESS = 0.35F;
constexpr float PLAYER_MODEL_SCALE = 0.28F;
constexpr const char* PLAYER_MODEL_PATH = "assets/character-a.glb";

constexpr Color PLAYER_ARROW_COLOR = {255, 255, 255, 230};
constexpr Color PLAYER_LABEL_COLOR = {255, 255, 255, 255};
constexpr Color PLAYER_LABEL_SHADOW_COLOR = {0, 0, 0, 200};

constexpr std::array<Color, 8> TEAM_COLORS = {{
    Color{230, 60, 60, 255},
    Color{60, 120, 230, 255},
    Color{60, 200, 80, 255},
    Color{240, 200, 0, 255},
    Color{240, 130, 0, 255},
    Color{180, 60, 220, 255},
    Color{0, 210, 210, 255},
    Color{220, 220, 220, 255},
}};

}  // namespace zappy::gui::config
