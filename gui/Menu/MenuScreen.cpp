/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** MenuScreen
*/

#include "Menu/MenuScreen.hpp"
#include <raylib.h>
#include <string>
#include <vector>
#include "Menu/MenuConfig.hpp"
#include "Render/TileGridRenderer.hpp"
#include "Render/WindowConfig.hpp"
#include "Render/raygui.h"
#include "World/WorldState.hpp"

namespace zappy::gui {

namespace cfg = menu::config;
namespace wcfg = config;

namespace {

WorldState buildSimulationWorld() {
  constexpr int mapSize = cfg::simulationMapSize;
  WorldState world;
  world.width = mapSize;
  world.height = mapSize;
  world.tiles.resize(static_cast<std::size_t>(world.height),
                     std::vector<Tile>(static_cast<std::size_t>(world.width)));
  for (int tileY = 0; tileY < world.height; ++tileY) {
    for (int tileX = 0; tileX < world.width; ++tileX) {
      world.tiles[static_cast<std::size_t>(tileY)]
                 [static_cast<std::size_t>(tileX)] =
          Tile{.x = tileX, .y = tileY, .resources = {}};
    }
  }
  return world;
}

Vector3 worldCenter(const WorldState& world) {
  return Vector3{static_cast<float>(world.width) * wcfg::TILE_SIZE / 2.0F, 0.0F,
                 static_cast<float>(world.height) * wcfg::TILE_SIZE / 2.0F};
}

std::string assetPath(const char* filename) {
  return std::string(ASSETS_DIR) + "/menu/" + filename;
}

}  // namespace

MenuScreen::MenuScreen()
    : world_(buildSimulationWorld()),
      camera_(worldCenter(world_)),
      playButton_(assetPath("btn_play.png").c_str(),
                  Rectangle{cfg::buttonX, cfg::playButtonY, cfg::buttonWidth,
                            cfg::buttonHeight}),
      controlsButton_(assetPath("btn_controls.png").c_str(),
                      Rectangle{cfg::buttonX, cfg::controlsButtonY,
                                cfg::buttonWidth, cfg::buttonHeight}),
      exitButton_(assetPath("btn_exit.png").c_str(),
                  Rectangle{cfg::buttonX, cfg::exitButtonY, cfg::buttonWidth,
                            cfg::buttonHeight}) {}

std::optional<GuiConfig> MenuScreen::run() {
  while (!WindowShouldClose()) {
    camera_.update(GetFrameTime());

    BeginDrawing();
    ClearBackground(BLACK);

    drawSimulation();
    DrawRectangle(0, 0, wcfg::WINDOW_WIDTH, wcfg::WINDOW_HEIGHT,
                  Color{0, 0, 0, 110});
    drawTitle();

    const bool clicksActive = (state_ == State::Menu);

    if (playButton_.draw() && clicksActive) {
      state_ = State::ConnectDialog;
    }
    if (controlsButton_.draw() && clicksActive) {
      state_ = State::ShowControls;
    }
    if (exitButton_.draw() && clicksActive) {
      EndDrawing();
      return std::nullopt;
    }

    if (state_ == State::ConnectDialog) {
      const DialogResult result = portDialog_.draw();
      if (result == DialogResult::Cancelled) {
        state_ = State::Menu;
      } else if (result == DialogResult::Connected) {
        EndDrawing();
        return portDialog_.result();
      }
    } else if (state_ == State::ShowControls) {
      drawControlsDialog();
    }

    EndDrawing();
  }
  return std::nullopt;
}

void MenuScreen::drawSimulation() {
  BeginMode3D(camera_.camera());
  TileGridRenderer::draw(world_);
  EndMode3D();
}

void MenuScreen::drawTitle() const {
  const char* title = "ZAPPY";
  const int titleWidth = MeasureText(title, cfg::titleFontSize);
  DrawText(title, (wcfg::WINDOW_WIDTH - titleWidth) / 2,
           static_cast<int>(cfg::titleY), cfg::titleFontSize, WHITE);
}

void MenuScreen::showConnectionError(const std::string& message) {
  state_ = State::ConnectDialog;
  portDialog_.setConnectionError(message);
}

void MenuScreen::reset() { state_ = State::Menu; }

void MenuScreen::drawControlsDialog() {
  DrawRectangle(0, 0, wcfg::WINDOW_WIDTH, wcfg::WINDOW_HEIGHT,
                Color{0, 0, 0, static_cast<unsigned char>(cfg::overlayAlpha)});

  const int dialogX = (wcfg::WINDOW_WIDTH - cfg::controlsDialogWidth) / 2;
  const int dialogY = (wcfg::WINDOW_HEIGHT - cfg::controlsDialogHeight) / 2;

  const int result = GuiMessageBox(
      Rectangle{static_cast<float>(dialogX), static_cast<float>(dialogY),
                static_cast<float>(cfg::controlsDialogWidth),
                static_cast<float>(cfg::controlsDialogHeight)},
      "Controls",
      "Camera movement:  WASD / Arrow keys\n"
      "Zoom:             Mouse wheel\n"
      "Rotate view:      Right-click drag\n"
      "Select player:    Left-click\n"
      "Speed +/-:        PageUp / PageDown\n"
      "Back to menu:     Escape",
      "Close");

  if (result >= 0) {
    state_ = State::Menu;
  }
}

}  // namespace zappy::gui
