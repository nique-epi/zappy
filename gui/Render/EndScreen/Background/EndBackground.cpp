/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** EndBackground — dark overlay and title for the end-of-game screen
*/

#include "Render/EndScreen/Background/EndBackground.hpp"
#include <raylib.h>
#include <string>
#include "Render/EndScreen/EndScreenConfig.hpp"
#include "Render/WindowConfig.hpp"

namespace zappy::gui {

namespace cfg = endscreen::config;
namespace wcfg = config;

namespace {

void drawTextCentered(const Font& font, const char* text, float fontSize,
                      Color color, float y) {
  const Vector2 size = MeasureTextEx(font, text, fontSize, 0.0F);
  const Vector2 pos{(static_cast<float>(wcfg::WINDOW_WIDTH) - size.x) / 2.0F,
                    y};
  DrawTextEx(font, text, pos, fontSize, 0.0F, color);
}

}  // namespace

void drawEndOverlay() {
  DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), cfg::overlayColor);
}

void drawEndTitle(const Font& font, const std::string& winnerTeam) {
  const auto fontSize = static_cast<float>(cfg::titleFontSize);
  drawTextCentered(font, "GAME OVER", fontSize, cfg::titleColor, cfg::titleY);
  const std::string sub = "Winner: " + winnerTeam;
  const float subY = cfg::titleY + fontSize + 12.0F;
  const float subSize = fontSize * 0.36F;
  drawTextCentered(font, sub.c_str(), subSize, cfg::winColor, subY);
}

void drawEndHint(const Font& font, const std::string& keyName) {
  const std::string hint = "Press " + keyName + " to return to menu";
  const float y = static_cast<float>(GetScreenHeight()) -
                  cfg::hintBottomMargin - static_cast<float>(cfg::hintFontSize);
  const Vector2 size =
      MeasureTextEx(font, hint.c_str(), static_cast<float>(cfg::hintFontSize),
                    cfg::textSpacing);
  const Vector2 pos{(static_cast<float>(GetScreenWidth()) - size.x) / 2.0F, y};
  DrawTextEx(font, hint.c_str(), pos, static_cast<float>(cfg::hintFontSize),
             cfg::textSpacing, cfg::hintColor);
}

}  // namespace zappy::gui
