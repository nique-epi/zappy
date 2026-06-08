/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** main
*/

#include <raylib.h>
#include <format>
#include <iostream>
#include <string>
#include <string_view>
#include "Cli/ArgsParser.hpp"
#include "Cli/Exceptions/ParserException.hpp"
#include "NetworkManager.hpp"
#include "ServerHandshake.hpp"
#include "WindowConfig.hpp"
#include "WorldState.hpp"
#include "raygui.h"

namespace cfg = zappy::gui::config;

static constexpr std::string_view USAGE =
    "USAGE: ./zappy_gui -p port -h machine";
static constexpr int EXIT_CODE_ERROR = 84;

int main(int argc, char** argv) {
  for (int i = 1; i < argc; ++i) {
    if (std::string_view(argv[i]) == "--help") {
      std::cout << USAGE << '\n';
      return 0;
    }
  }

  try {
    const zappy::gui::GuiConfig config = zappy::gui::parseArguments(argc, argv);
    zappy::gui::NetworkManager network(config.hostname, config.port);
    zappy::gui::ServerHandshake handshake(network);

    InitWindow(cfg::WINDOW_WIDTH, cfg::WINDOW_HEIGHT, cfg::WINDOW_TITLE);
    SetTargetFPS(cfg::TARGET_FPS);

    const zappy::gui::WorldState world = zappy::gui::mockWorld();

    while (!WindowShouldClose()) {
      network.runOnce(0);
      handshake.checkTimeout();

      const std::string hudText =
          std::format("{}:{}  |  Map: {}x{}  |  Players: {}  |  Teams: {}",
                      config.hostname, config.port, world.width, world.height,
                      world.players.size(), world.teams.size());

      BeginDrawing();
      ClearBackground(RAYWHITE);
      DrawText(cfg::WINDOW_TITLE, cfg::MARGIN_X, cfg::TITLE_Y,
               cfg::TITLE_FONT_SIZE, DARKBLUE);
      DrawText(hudText.c_str(), cfg::MARGIN_X, cfg::HUD_Y, cfg::HUD_FONT_SIZE,
               DARKGRAY);
      EndDrawing();
    }

    CloseWindow();
  } catch (const zappy::gui::ParserException& error) {
    std::cerr << error.what() << '\n';
    std::cerr << USAGE << '\n';
    return EXIT_CODE_ERROR;
  } catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    return EXIT_CODE_ERROR;
  } catch (...) {
    std::cerr << "An unknown error occurred.\n";
    return EXIT_CODE_ERROR;
  }
  return 0;
}
