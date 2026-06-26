/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** KeyBindings — rebindable keyboard actions
*/

#pragma once  // NOLINT(llvm-header-guard)

#include <raylib.h>
#include <string>

namespace zappy::gui {

struct KeyBindings {
  int cameraForward{KEY_W};
  int cameraBackward{KEY_S};
  int cameraLeft{KEY_A};
  int cameraRight{KEY_D};
  int backToMenu{KEY_ESCAPE};
};

std::string keyName(int key);

}  // namespace zappy::gui
