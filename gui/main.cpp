/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** main
*/

#include <iostream>
#include "App/Application.hpp"
#include "Cli/ArgsParser.hpp"

int main(int argc, char** argv) {
  if (zappy::gui::helpRequested(argc, argv)) {
    std::cout << zappy::gui::usageMessage() << '\n';
    return 0;
  }
  return zappy::gui::Application(argc, argv).run();
}
