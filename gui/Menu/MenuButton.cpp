/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** MenuButton
*/

#include "Menu/MenuButton.hpp"
#include <raylib.h>

namespace zappy::gui {

MenuButton::MenuButton(const char* texturePath, Rectangle bounds)
    : bounds_(bounds) {
  texture_ = LoadTexture(texturePath);
  loaded_ = (texture_.id > 0);
}

MenuButton::~MenuButton() {
  if (loaded_) {
    UnloadTexture(texture_);
  }
}

bool MenuButton::draw() {
  const Vector2 mouse = GetMousePosition();
  const bool hovered = CheckCollisionPointRec(mouse, bounds_);
  const Color tint = hovered ? Color{210, 210, 210, 255} : WHITE;

  if (loaded_) {
    const Rectangle source{0.0F, 0.0F, static_cast<float>(texture_.width),
                           static_cast<float>(texture_.height)};
    DrawTexturePro(texture_, source, bounds_, Vector2{0.0F, 0.0F}, 0.0F, tint);
  } else {
    DrawRectangleRec(bounds_, hovered ? LIGHTGRAY : DARKGRAY);
  }

  return hovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
}

}  // namespace zappy::gui
