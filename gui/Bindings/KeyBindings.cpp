/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** KeyBindings
*/

#include "Bindings/KeyBindings.hpp"
#include <raylib.h>
#include <string>
#include <unordered_map>

namespace zappy::gui {

std::string keyName(int key) {  // NOLINT(misc-use-internal-linkage)
  static const std::unordered_map<int, std::string> names{
      {KEY_NULL, "None"},
      {KEY_SPACE, "Space"},
      {KEY_ESCAPE, "Escape"},
      {KEY_ENTER, "Enter"},
      {KEY_TAB, "Tab"},
      {KEY_BACKSPACE, "Backspace"},
      {KEY_INSERT, "Insert"},
      {KEY_DELETE, "Delete"},
      {KEY_RIGHT, "Right"},
      {KEY_LEFT, "Left"},
      {KEY_DOWN, "Down"},
      {KEY_UP, "Up"},
      {KEY_PAGE_UP, "PageUp"},
      {KEY_PAGE_DOWN, "PageDown"},
      {KEY_HOME, "Home"},
      {KEY_END, "End"},
      {KEY_F1, "F1"},
      {KEY_F2, "F2"},
      {KEY_F3, "F3"},
      {KEY_F4, "F4"},
      {KEY_F5, "F5"},
      {KEY_F6, "F6"},
      {KEY_F7, "F7"},
      {KEY_F8, "F8"},
      {KEY_F9, "F9"},
      {KEY_F10, "F10"},
      {KEY_F11, "F11"},
      {KEY_F12, "F12"},
      {KEY_LEFT_SHIFT, "L.Shift"},
      {KEY_LEFT_CONTROL, "L.Ctrl"},
      {KEY_LEFT_ALT, "L.Alt"},
      {KEY_RIGHT_SHIFT, "R.Shift"},
      {KEY_RIGHT_CONTROL, "R.Ctrl"},
      {KEY_RIGHT_ALT, "R.Alt"},
  };

  const auto found = names.find(key);
  if (found != names.end()) {
    return found->second;
  }
  if (key >= KEY_A && key <= KEY_Z) {
    return std::string(
        1,
        static_cast<char>(key));  // NOLINT(modernize-return-braced-init-list)
  }
  if (key >= KEY_ZERO && key <= KEY_NINE) {
    return std::string(
        1,
        static_cast<char>(key));  // NOLINT(modernize-return-braced-init-list)
  }
  return "Key" + std::to_string(key);
}

}  // namespace zappy::gui
