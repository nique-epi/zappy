/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** main
*/

#include <iostream>
#include "App/Application.hpp"
#include "Cli/LaunchMode.hpp"

int main(int argc, char** argv) {
  if (zappy::gui::helpCommand(argc, argv)) {
    std::cout << zappy::gui::GUI_USAGE << '\n';
    return 0;
  }
  return zappy::gui::Application(argc, argv).run();
}
