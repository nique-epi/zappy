/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** MenuButton — rounded beige button with brown border
*/

#pragma once  // NOLINT(llvm-header-guard)

#include <raylib.h>
#include <string_view>

namespace zappy::gui {

class MenuButton {
 public:
  MenuButton(std::string_view label, Rectangle bounds, Font font,
             float fontSize);

  /**
   * @brief Draws the button and returns true if clicked this frame.
   */
  [[nodiscard]] bool draw() const;

 private:
  std::string_view label_{};
  Rectangle bounds_{};
  Font font_{};
  float fontSize_{};
};

}  // namespace zappy::gui
