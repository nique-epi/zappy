/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** main
*/

#include <raylib.h>
#include <exception>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include "Bindings/KeyBindings.hpp"
#include "Cli/ArgsParser.hpp"                  // NOLINT(misc-include-cleaner)
#include "Cli/Exceptions/ParserException.hpp"  // NOLINT(misc-include-cleaner)
#include "Menu/MenuScreen.hpp"
#include "Network/CommandSender.hpp"
#include "Network/Exceptions.hpp"  // NOLINT(misc-include-cleaner)
#include "Network/NetworkManager.hpp"
#include "Network/Parsing/MessageParser.hpp"
#include "Network/ServerHandshake.hpp"
#include "Render/Entity/EggRenderer.hpp"
#include "Render/Entity/IncantationRenderer.hpp"
#include "Render/Entity/PlayerChevron.hpp"
#include "Render/Entity/PlayerPicker.hpp"
#include "Render/Entity/PlayerRenderer.hpp"
#include "Render/Entity/PlayerSelection.hpp"
#include "Render/Entity/ResourceRenderer.hpp"
#include "Render/Panel/HudPanel.hpp"
#include "Render/Panel/InfoPanel.hpp"
#include "Render/Panel/PlayerPanel.hpp"
#include "Render/SpeedControl.hpp"
#include "Render/TileGridRenderer.hpp"
#include "Render/WindowConfig.hpp"
#include "Render/WorldCamera.hpp"
#include "Render/raygui.h"
#include "World/WorldInitializer.hpp"
#include "World/WorldState.hpp"

namespace cfg = zappy::gui::config;

namespace {

constexpr std::string_view USAGE = "USAGE: ./zappy_gui -p port -h machine";
constexpr int EXIT_CODE_ERROR = 84;
constexpr int STARTUP_POLL_TIMEOUT_MS = 50;

bool hasBothCLIFlags(int argc, char** argv) {
  bool hasPort = false;
  bool hasHost = false;
  for (int i = 1; i < argc - 1; ++i) {
    if (std::string_view(argv[i]) == "-p") {
      hasPort = true;
    }
    if (std::string_view(argv[i]) == "-h") {
      hasHost = true;
    }
  }
  return hasPort && hasHost;
}

bool runHandshakeAndInit(zappy::gui::NetworkManager& network,
                         zappy::gui::WorldState& world,
                         zappy::gui::MessageParser& parser) {
  const zappy::gui::ServerHandshake handshake(network);
  while (handshake.status() != zappy::gui::HandshakeStatus::Done) {
    if (WindowShouldClose()) {
      return false;
    }
    network.runOnce(STARTUP_POLL_TIMEOUT_MS);
    handshake.checkTimeout();
  }

  {
    zappy::gui::WorldInitializer initializer(network, parser, world);
    while (!initializer.isDone()) {
      if (WindowShouldClose()) {
        return false;
      }
      network.runOnce(STARTUP_POLL_TIMEOUT_MS);
      initializer.checkTimeout();
      initializer.onPollRoundComplete();
    }
  }
  network.setResponseHandler(
      [&parser](const std::string& line) { parser.parseLine(line); });
  return true;
}

}  // namespace

int main(int argc, char** argv) {
  for (int i = 1; i < argc; ++i) {
    if (std::string_view(argv[i]) == "--help") {
      std::cout << USAGE << '\n';
      return 0;
    }
  }

  try {
    InitWindow(cfg::WINDOW_WIDTH, cfg::WINDOW_HEIGHT, cfg::WINDOW_TITLE);
    SetTargetFPS(cfg::TARGET_FPS);
    SetExitKey(KEY_NULL);

    bool useCLI = hasBothCLIFlags(argc, argv);
    zappy::gui::MenuScreen menu;

    while (!WindowShouldClose()) {
      std::optional<zappy::gui::GuiConfig> configOpt;
      std::unique_ptr<zappy::gui::NetworkManager> networkPtr;

      if (useCLI) {
        useCLI = false;
        configOpt = zappy::gui::parseArguments(argc, argv);
        try {
          networkPtr = std::make_unique<zappy::gui::NetworkManager>(
              configOpt->hostname, configOpt->port);
        } catch (const zappy::network::ConnectError& connectError) {
          menu.showConnectionError(connectError.what());
          configOpt = std::nullopt;
        }
      }

      if (!configOpt) {
        while (!WindowShouldClose()) {
          configOpt = menu.run();
          if (!configOpt) {
            break;
          }
          try {
            networkPtr = std::make_unique<zappy::gui::NetworkManager>(
                configOpt->hostname, configOpt->port);
            break;
          } catch (const zappy::network::ConnectError& connectError) {
            menu.showConnectionError(connectError.what());
          }
        }
        if (!configOpt) {
          break;
        }
      }

      const zappy::gui::GuiConfig config = *configOpt;
      zappy::gui::NetworkManager& network = *networkPtr;
      const zappy::gui::KeyBindings bindings = menu.keyBindings();

      zappy::gui::WorldState world;
      zappy::gui::MessageParser parser(world);
      if (!runHandshakeAndInit(network, world, parser)) {
        break;
      }

      zappy::gui::CommandSender sender(network);
      zappy::gui::SpeedControl speedControl(sender);
      sender.requestTimeUnit();

      zappy::gui::IncantationRenderer::loadTextures();
      zappy::gui::ResourceRenderer::loadModels();

      const Vector3 mapCenter{
          static_cast<float>(world.width) * cfg::TILE_SIZE / 2.0F, 0.0F,
          static_cast<float>(world.height) * cfg::TILE_SIZE / 2.0F};
      zappy::gui::WorldCamera camera(mapCenter);
      zappy::gui::PlayerSelection selection;

      bool returnToMenu = false;
      while (!WindowShouldClose()) {
        if (IsKeyPressed(bindings.backToMenu)) {
          returnToMenu = true;
          break;
        }

        network.runOnce(0);
        camera.update(GetFrameTime(), bindings);

        selection.syncWithWorld(world);
        const std::optional<int> hoveredPlayer =
            zappy::gui::PlayerPicker::nearest(world, camera.camera());
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
          const bool insidePanel =
              selection.selectedId().has_value() &&
              zappy::gui::PlayerPanel::contains(GetMousePosition());
          const std::optional<int> picked =
              selection.click(hoveredPlayer, insidePanel);
          if (picked.has_value()) {
            sender.requestPlayerInventory(*picked);
            sender.requestPlayerLevel(*picked);
          }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera.camera());
        zappy::gui::TileGridRenderer::draw(world);
        zappy::gui::ResourceRenderer::draw3D(world);
        zappy::gui::EggRenderer::draw3D(world);
        zappy::gui::PlayerRenderer::draw3D(world);
        zappy::gui::IncantationRenderer::draw3D(world);
        EndMode3D();
        zappy::gui::PlayerRenderer::drawLevelLabels(world, camera.camera());
        zappy::gui::PlayerChevron::draw(world, camera.camera(), hoveredPlayer,
                                        selection.selectedId());

        zappy::gui::HudPanel::draw(config, world);
        zappy::gui::InfoPanel::draw(world, camera.camera());
        speedControl.draw(world.timeUnit);
        if (selection.selectedId().has_value()) {
          if (zappy::gui::PlayerPanel::draw(world, *selection.selectedId())) {
            selection.close();
          }
        }
        EndDrawing();
      }

      zappy::gui::IncantationRenderer::unloadTextures();
      zappy::gui::ResourceRenderer::unloadModels();
      networkPtr.reset();

      if (!returnToMenu) {
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
    std::cerr << USAGE << '\n';
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
