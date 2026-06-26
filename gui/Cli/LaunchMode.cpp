/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** LaunchMode — how the GUI was launched (CLI connection arguments)
*/

#include "Cli/LaunchMode.hpp"
#include <string_view>

namespace zappy::gui {

bool hasConnectionArguments(int argumentCount, char** arguments) {
  bool hasPort = false;
  bool hasHost = false;

  for (int i = 1; i < argumentCount - 1; ++i) {
    if (std::string_view(arguments[i]) == "-p") {
      hasPort = true;
    }
    if (std::string_view(arguments[i]) == "-h") {
      hasHost = true;
    }
  }
  return hasPort && hasHost;
}

}  // namespace zappy::gui
