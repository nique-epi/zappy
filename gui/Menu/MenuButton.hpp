/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** MenuButton — texture-based button with hover and click detection
*/

#pragma once

#include <raylib.h>

namespace zappy::gui {

class MenuButton {
 public:
  MenuButton(const char* texturePath, Rectangle bounds);
  ~MenuButton();

  MenuButton(const MenuButton&) = delete;
  MenuButton& operator=(const MenuButton&) = delete;
  MenuButton(MenuButton&&) = delete;
  MenuButton& operator=(MenuButton&&) = delete;

  /**
   * @brief Draws the button and returns true if clicked this frame.
   */
  bool draw();

 private:
  Texture2D texture_{};
  Rectangle bounds_{};
  bool loaded_{false};
};

}  // namespace zappy::gui
