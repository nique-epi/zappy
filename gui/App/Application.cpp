/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Application — owns the window and the menu/session orchestration loop
*/

#include "App/Application.hpp"
#include <raylib.h>
#include <exception>
#include <iostream>
#include "App/GameSession.hpp"
#include "Cli/ArgsParser.hpp"
#include "Cli/Exceptions/ParserException.hpp"
#include "Cli/LaunchMode.hpp"
#include "Menu/MenuScreen.hpp"
#include "Network/SessionConnector.hpp"
#include "Render/WindowConfig.hpp"

namespace zappy::gui {

namespace cfg = config;

namespace {

constexpr int EXIT_CODE_ERROR = 84;

}  // namespace

Application::Application(int argumentCount, char** arguments)
    : argumentCount_(argumentCount), arguments_(arguments) {}

int Application::run() {
  try {
    InitWindow(cfg::WINDOW_WIDTH, cfg::WINDOW_HEIGHT, cfg::WINDOW_TITLE);
    SetTargetFPS(cfg::TARGET_FPS);
    SetExitKey(KEY_NULL);

    bool useCLI = hasConnectionArguments(argumentCount_, arguments_);
    MenuScreen menu;

    while (!WindowShouldClose()) {
      Session session;
      if (useCLI) {
        useCLI = false;
        session = connectFromArguments(argumentCount_, arguments_, menu);
      }
      if (!session.config) {
        session = connectFromMenu(menu);
        if (!session.config) {
          break;
        }
      }

      GameSession game(*session.config, *session.network, menu.keyBindings());
      if (!game.run()) {
        break;
      }
      menu.reset();
    }

    CloseWindow();
  } catch (const zappy::cli::ParserException& error) {
    if (IsWindowReady()) {
      CloseWindow();
    }
    std::cerr << error.what() << '\n';
    std::cerr << usageMessage() << '\n';
    return EXIT_CODE_ERROR;
  } catch (const std::exception& error) {
    if (IsWindowReady()) {
      CloseWindow();
    }
    std::cerr << error.what() << '\n';
    return EXIT_CODE_ERROR;
  } catch (...) {
    if (IsWindowReady()) {
      CloseWindow();
    }
    std::cerr << "An unknown error occurred.\n";
    return EXIT_CODE_ERROR;
  }
  return 0;
}

}  // namespace zappy::gui
