/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** MenuScreen
*/

#include "Menu/MenuScreen.hpp"
#include <raylib.h>
#include <cstddef>
#include <optional>
#include <string>
#include <vector>
#include "Bindings/KeyBindings.hpp"
#include "GuiConfig.hpp"
#include "Menu/KeyBindingsDialog.hpp"
#include "Menu/MenuConfig.hpp"
#include "Menu/PortInputDialog.hpp"
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

constexpr float half = 2.0F;

Vector3 worldCenter(const WorldState& world) {
  return Vector3{static_cast<float>(world.width) * wcfg::TILE_SIZE / half, 0.0F,
                 static_cast<float>(world.height) * wcfg::TILE_SIZE / half};
}

std::string assetPath(const char* filename) {
  return std::string(ASSETS_DIR) + "/menu/" + filename;
}

}  // namespace

MenuScreen::~MenuScreen() {
  UnloadFont(titleFont_);
  if (skyTex_.id > 0) {
    UnloadTexture(skyTex_);
  }
}

MenuScreen::MenuScreen()
    : world_(buildSimulationWorld()),
      camera_(worldCenter(world_)),
      titleFont_(LoadFontEx(assetPath("KiwiSoda.ttf").c_str(),
                            cfg::titleFontSize * 2, nullptr, 0)),
      playButton_("Play",
                  Rectangle{cfg::buttonX, cfg::playButtonY, cfg::buttonWidth,
                            cfg::buttonHeight},
                  titleFont_, cfg::buttonFontSize),
      controlsButton_("Controls",
                      Rectangle{cfg::buttonX, cfg::controlsButtonY,
                                cfg::buttonWidth, cfg::buttonHeight},
                      titleFont_, cfg::buttonFontSize),
      exitButton_("Exit",
                  Rectangle{cfg::buttonX, cfg::exitButtonY, cfg::buttonWidth,
                            cfg::buttonHeight},
                  titleFont_, cfg::buttonFontSize) {
  Image hdr = LoadImage(assetPath("sky.hdr").c_str());
  if (hdr.data != nullptr) {
    if (hdr.format == PIXELFORMAT_UNCOMPRESSED_R32G32B32) {
      constexpr float exposure = 1.5F;
      constexpr int rgbChannels = 3;
      auto* pixels = static_cast<float*>(hdr.data);
      const int count = hdr.width * hdr.height * rgbChannels;
      for (int i = 0; i < count; ++i) {
        const float v = pixels[i] * exposure;
        pixels[i] = v / (v + 1.0F);
      }
    }
    ImageFormat(&hdr, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    ImageResize(&hdr, wcfg::WINDOW_WIDTH, wcfg::WINDOW_HEIGHT);
    skyTex_ = LoadTextureFromImage(hdr);
    SetTextureFilter(skyTex_, TEXTURE_FILTER_BILINEAR);
    UnloadImage(hdr);
  }
}

std::optional<GuiConfig> MenuScreen::run() {
  while (!WindowShouldClose()) {
    camera_.update(GetFrameTime());

    BeginDrawing();
    ClearBackground(BLACK);

    drawSimulation();
    DrawRectangle(0, 0, wcfg::WINDOW_WIDTH, wcfg::WINDOW_HEIGHT,
                  Color{0, 0, 0, cfg::menuBackgroundOverlay});
    drawTitle();

    const bool clicksActive = (state_ == State::Menu);

    if (playButton_.draw() && clicksActive) {
      portDialog_.open();
      state_ = State::ConnectDialog;
    }
    if (controlsButton_.draw() && clicksActive) {
      kbDialog_.open();
      state_ = State::EditBindings;
    }
    if (exitButton_.draw() && clicksActive) {
      EndDrawing();
      return std::nullopt;
    }

    if (state_ == State::ConnectDialog) {
      const DialogResult result =  // NOLINT(cppcoreguidelines-init-variables)
          portDialog_.draw();
      if (result == DialogResult::Cancelled) {
        state_ = State::Menu;
      } else if (result == DialogResult::Connected) {
        EndDrawing();
        return portDialog_.result();
      }
    } else if (state_ == State::EditBindings) {
      if (kbDialog_.draw() == BindingsResult::Closed) {
        state_ = State::Menu;
      }
    }

    EndDrawing();
  }
  return std::nullopt;
}

void MenuScreen::drawSimulation() {
  if (skyTex_.id > 0) {
    const Rectangle src{0.0F, 0.0F, static_cast<float>(skyTex_.width),
                        static_cast<float>(skyTex_.height)};
    const Rectangle dst{0.0F, 0.0F, static_cast<float>(wcfg::WINDOW_WIDTH),
                        static_cast<float>(wcfg::WINDOW_HEIGHT)};
    DrawTexturePro(skyTex_, src, dst, {0.0F, 0.0F}, 0.0F, WHITE);
  }

  BeginMode3D(camera_.camera());
  TileGridRenderer::draw(world_);
  EndMode3D();
}

void MenuScreen::drawTitle() const {
  const char* title = "ZAPPY";
  const auto fontSize = static_cast<float>(cfg::titleFontSize);
  const Vector2 size = MeasureTextEx(titleFont_, title, fontSize, 0.0F);
  const Vector2 pos{(static_cast<float>(wcfg::WINDOW_WIDTH) - size.x) / 2.0F,
                    cfg::titleY};
  DrawTextEx(titleFont_, title, pos, fontSize, 0.0F, WHITE);
}

void MenuScreen::showConnectionError(const std::string& message) {
  state_ = State::ConnectDialog;
  portDialog_.setConnectionError(message);
}

void MenuScreen::reset() { state_ = State::Menu; }

const KeyBindings& MenuScreen::keyBindings() const {
  return kbDialog_.result();
}

}  // namespace zappy::gui
