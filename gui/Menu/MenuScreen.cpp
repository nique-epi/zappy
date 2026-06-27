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
#include "Menu/Dialogs/KeyBindingsDialog.hpp"
#include "Menu/Dialogs/PortInputDialog.hpp"
#include "Menu/MenuConfig.hpp"
#include "Render/SkyRenderer.hpp"
#include "Render/TileGridRenderer.hpp"
#include "Render/WindowConfig.hpp"
#include "Render/raygui.h"
#include "Theme/Theme.hpp"
#include "World/WorldState.hpp"

namespace zappy::gui {

namespace cfg = menu::config;
namespace wcfg = config;

namespace {

constexpr Color themeBeige = theme::parchment;
constexpr Color themeBeigeLight = theme::parchmentLight;
constexpr Color themedBeigeFocus = theme::parchmentHover;
constexpr Color themeBeigePress = theme::parchmentPressed;
constexpr Color themeBrown = theme::inkBrown;
constexpr Color themeBrownDark = theme::inkBrownDark;

void applyMenuTheme() {
  GuiSetStyle(DEFAULT, BACKGROUND_COLOR,
              static_cast<int>(ColorToInt(themeBeige)));
  GuiSetStyle(DEFAULT, LINE_COLOR, static_cast<int>(ColorToInt(themeBrown)));
  GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL,
              static_cast<int>(ColorToInt(themeBrown)));
  GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL,
              static_cast<int>(ColorToInt(themeBeige)));
  GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL,
              static_cast<int>(ColorToInt(themeBrownDark)));
  GuiSetStyle(DEFAULT, BORDER_WIDTH, 2);
  GuiSetStyle(DEFAULT, TEXT_SIZE, 16);

  GuiSetStyle(STATUSBAR, BASE_COLOR_NORMAL,
              static_cast<int>(ColorToInt(themeBeigePress)));
  GuiSetStyle(STATUSBAR, BORDER_COLOR_NORMAL,
              static_cast<int>(ColorToInt(themeBrown)));
  GuiSetStyle(STATUSBAR, TEXT_COLOR_NORMAL,
              static_cast<int>(ColorToInt(themeBrownDark)));

  GuiSetStyle(BUTTON, BASE_COLOR_NORMAL,
              static_cast<int>(ColorToInt(themeBeige)));
  GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED,
              static_cast<int>(ColorToInt(themedBeigeFocus)));
  GuiSetStyle(BUTTON, BASE_COLOR_PRESSED,
              static_cast<int>(ColorToInt(themeBeigePress)));
  GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL,
              static_cast<int>(ColorToInt(themeBrown)));
  GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED,
              static_cast<int>(ColorToInt(themeBrown)));
  GuiSetStyle(BUTTON, BORDER_COLOR_PRESSED,
              static_cast<int>(ColorToInt(themeBrownDark)));
  GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL,
              static_cast<int>(ColorToInt(themeBrownDark)));
  GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED,
              static_cast<int>(ColorToInt(themeBrownDark)));
  GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED,
              static_cast<int>(ColorToInt(themeBrownDark)));

  GuiSetStyle(TEXTBOX, BASE_COLOR_NORMAL,
              static_cast<int>(ColorToInt(themeBeigeLight)));
  GuiSetStyle(TEXTBOX, BASE_COLOR_FOCUSED,
              static_cast<int>(ColorToInt(themeBeigeLight)));
  GuiSetStyle(TEXTBOX, BORDER_COLOR_NORMAL,
              static_cast<int>(ColorToInt(themeBrown)));
  GuiSetStyle(TEXTBOX, BORDER_COLOR_FOCUSED,
              static_cast<int>(ColorToInt(themeBrownDark)));
  GuiSetStyle(TEXTBOX, TEXT_COLOR_NORMAL,
              static_cast<int>(ColorToInt(themeBrownDark)));
  GuiSetStyle(TEXTBOX, TEXT_COLOR_FOCUSED,
              static_cast<int>(ColorToInt(themeBrownDark)));

  GuiSetStyle(LABEL, TEXT_COLOR_NORMAL,
              static_cast<int>(ColorToInt(themeBrownDark)));
  GuiSetStyle(LABEL, TEXT_COLOR_FOCUSED,
              static_cast<int>(ColorToInt(themeBrownDark)));
}

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
  SkyRenderer::unload();
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
  SkyRenderer::load();
  applyMenuTheme();
}

bool MenuScreen::drawButtons() {
  const bool active = (state_ == State::Menu);
  if (playButton_.draw() && active) {
    portDialog_.open();
    state_ = State::ConnectDialog;
  }
  if (controlsButton_.draw() && active) {
    kbDialog_.open();
    state_ = State::EditBindings;
  }
  return exitButton_.draw() && active;
}

std::optional<GuiConfig> MenuScreen::tickActiveDialog() {
  switch (state_) {
    case State::ConnectDialog: {
      const DialogResult result =  // NOLINT(cppcoreguidelines-init-variables)
          portDialog_.draw();
      if (result == DialogResult::Cancelled) {
        state_ = State::Menu;
      } else if (result == DialogResult::Connected) {
        return portDialog_.result();
      }
      break;
    }
    case State::EditBindings:
      if (kbDialog_.draw() == BindingsResult::Closed) {
        state_ = State::Menu;
      }
      break;
    case State::Menu:
      break;
  }
  return std::nullopt;
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

    const bool wantsExit = drawButtons();
    const auto connected = tickActiveDialog();
    EndDrawing();

    if (wantsExit) {
      return std::nullopt;
    }
    if (connected) {
      return connected;
    }
  }
  return std::nullopt;
}

void MenuScreen::drawSimulation() {
  BeginMode3D(camera_.camera());
  SkyRenderer::draw(camera_.camera());
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
