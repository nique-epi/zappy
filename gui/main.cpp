/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** main
*/

#include <raylib.h>
#include <algorithm>
#include <format>
#include <iostream>
#include <string>
#include <string_view>
#include "Cli/ArgsParser.hpp"
#include "Cli/Exceptions/ParserException.hpp"  // NOLINT(misc-include-cleaner)
#include "Network/NetworkManager.hpp"
#include "Network/Parsing/MessageParser.hpp"
#include "Network/ServerHandshake.hpp"
#include "Render/WindowConfig.hpp"
#include "Render/WorldCamera.hpp"
#include "Render/raygui.h"
#include "World/WorldInitializer.hpp"
#include "World/WorldState.hpp"

namespace cfg = zappy::gui::config;

static constexpr std::string_view USAGE =
    "USAGE: ./zappy_gui -p port -h machine";
static constexpr int EXIT_CODE_ERROR = 84;
static constexpr int STARTUP_POLL_TIMEOUT_MS = 50;

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

    while (handshake.status() != zappy::gui::HandshakeStatus::Done) {
      network.runOnce(STARTUP_POLL_TIMEOUT_MS);
      handshake.checkTimeout();
    }

    zappy::gui::WorldState world;
    zappy::gui::MessageParser parser(world);
    {
      zappy::gui::WorldInitializer initializer(network, parser, world);
      while (!initializer.isDone()) {
        network.runOnce(STARTUP_POLL_TIMEOUT_MS);
        initializer.checkTimeout();
        initializer.onPollRoundComplete();
      }
    }
    network.setResponseHandler(
        [&parser](const std::string& line) { parser.parseLine(line); });

    InitWindow(cfg::WINDOW_WIDTH, cfg::WINDOW_HEIGHT, cfg::WINDOW_TITLE);
    SetTargetFPS(cfg::TARGET_FPS);

    const Vector3 mapCenter{
        static_cast<float>(world.width) * cfg::TILE_SIZE / 2.0F, 0.0F,
        static_cast<float>(world.height) * cfg::TILE_SIZE / 2.0F};
    zappy::gui::WorldCamera camera(mapCenter);

    while (!WindowShouldClose()) {
      network.runOnce(0);
      camera.update(GetFrameTime());

      const std::string hudText =
          std::format("{}:{}  |  Map: {}x{}  |  Players: {}  |  Teams: {}",
                      config.hostname, config.port, world.width, world.height,
                      world.players.size(), world.teams.size());

      BeginDrawing();
      ClearBackground(RAYWHITE);

      BeginMode3D(camera.camera());
      DrawGrid(static_cast<int>(std::max(world.width, world.height)),
               cfg::TILE_SIZE);
      EndMode3D();

      DrawText(cfg::WINDOW_TITLE, cfg::MARGIN_X, cfg::TITLE_Y,
               cfg::TITLE_FONT_SIZE, DARKBLUE);
      DrawText(hudText.c_str(), cfg::MARGIN_X, cfg::HUD_Y, cfg::HUD_FONT_SIZE,
               DARKGRAY);
      EndDrawing();
    }

    CloseWindow();
  } catch (const zappy::cli::ParserException& error) {
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
