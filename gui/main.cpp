/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** main
*/

#include <raylib.h>
#include <cstdlib>
#include <format>
#include <string>
#include "WindowConfig.hpp"
#include "WorldState.hpp"
#include "raygui.h"

namespace cfg = zappy::gui::config;

int main() {
  try {
    InitWindow(cfg::WINDOW_WIDTH, cfg::WINDOW_HEIGHT, cfg::WINDOW_TITLE);
    SetTargetFPS(cfg::TARGET_FPS);

    const zappy::gui::WorldState world = zappy::gui::mockWorld();

    while (!WindowShouldClose()) {
      const std::string hudText =
          std::format("Map: {}x{}  |  Players: {}  |  Teams: {}", world.width,
                      world.height, world.players.size(), world.teams.size());

      BeginDrawing();
      ClearBackground(RAYWHITE);
      DrawText(cfg::WINDOW_TITLE, cfg::MARGIN_X, cfg::TITLE_Y,
               cfg::TITLE_FONT_SIZE, DARKBLUE);
      DrawText(hudText.c_str(), cfg::MARGIN_X, cfg::HUD_Y, cfg::HUD_FONT_SIZE,
               DARKGRAY);
      EndDrawing();
    }

    CloseWindow();
  } catch (...) {
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
