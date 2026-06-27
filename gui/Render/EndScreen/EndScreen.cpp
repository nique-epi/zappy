/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** EndScreen — game-over overlay orchestrating overlay, title and team panel
*/

#include "Render/EndScreen/EndScreen.hpp"
#include <raylib.h>
#include <string>
#include "Render/EndScreen/Background/EndBackground.hpp"
#include "Render/EndScreen/EndScreenConfig.hpp"
#include "Render/EndScreen/Panel/TeamResultPanel.hpp"
#include "World/WorldState.hpp"

namespace zappy::gui {

namespace {

Font& font() {
  static Font instance;
  return instance;
}

std::string assetPath(const char* filename) {
  return std::string(ASSETS_DIR) + "/menu/" + filename;
}

}  // namespace

void EndScreen::loadFont() {
  font() = LoadFontEx(assetPath("KiwiSoda.ttf").c_str(),
                      endscreen::config::titleFontSize * 2, nullptr, 0);
}

void EndScreen::unloadFont() { UnloadFont(font()); }

void EndScreen::draw(const WorldState& world, const std::string& keyName) {
  drawEndOverlay();
  drawEndTitle(font(), world.winnerTeam);
  const Font defaultFont = GetFontDefault();
  TeamResultPanel::draw(world, defaultFont);
  drawEndHint(defaultFont, keyName);
}

}  // namespace zappy::gui
