/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Zappy server entry point — unified GUI + AI protocol shell
*/

#include <exception>
#include <iostream>
#include "App/ArgsParser.hpp"
#include "App/GameServer.hpp"

int main(int argc, char **argv) {
  try {
    const zappy::server::ServerConfig config =
        zappy::server::parseArguments(argc, argv);
    zappy::server::GameServer server(config);
    server.start();
    std::cout << "zappy server listening on port " << server.port() << '\n';
    server.run();
  } catch (const std::exception &error) {
    std::cerr << "fatal: " << error.what() << '\n';
    return 84;
  }
  return 0;
}
