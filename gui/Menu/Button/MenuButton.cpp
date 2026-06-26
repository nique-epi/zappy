/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** MenuButton
*/

#include "Menu/Button/MenuButton.hpp"
#include <raylib.h>
#include <string>
#include <string_view>
#include "Menu/MenuConfig.hpp"

namespace zappy::gui {

namespace cfg = menu::config;

namespace {

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
constexpr Color colorBg = {245, 232, 210, 255};
constexpr Color colorBgHover = {230, 215, 188, 255};
constexpr Color colorBgHeld = {210, 195, 165, 255};
constexpr Color colorBorder = {101, 67, 33, 255};
constexpr Color colorText = {80, 50, 25, 255};
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

}  // namespace

MenuButton::MenuButton(std::string_view label, Rectangle bounds, Font font,
                       float fontSize)
    : label_(label), bounds_(bounds), font_(font), fontSize_(fontSize) {}

bool MenuButton::draw() const {
  const Vector2 mouse = GetMousePosition();
  const bool hovered = CheckCollisionPointRec(mouse, bounds_);
  const bool held = hovered && IsMouseButtonDown(MOUSE_BUTTON_LEFT);

  const Color bg = held ? colorBgHeld : (hovered ? colorBgHover : colorBg);

  DrawRectangleRounded(bounds_, cfg::buttonRoundness, cfg::buttonSegments, bg);
  DrawRectangleRoundedLinesEx(bounds_, cfg::buttonRoundness,
                              cfg::buttonSegments, cfg::buttonBorderWidth,
                              colorBorder);

  const std::string label{label_};
  const Vector2 textSize = MeasureTextEx(font_, label.c_str(), fontSize_, 0.0F);
  const Vector2 textPos{
      bounds_.x + (bounds_.width - textSize.x) / 2.0F,
      bounds_.y + (bounds_.height - textSize.y) / 2.0F,
  };
  DrawTextEx(font_, label.c_str(), textPos, fontSize_, 0.0F, colorText);

  return hovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
}

}  // namespace zappy::gui
