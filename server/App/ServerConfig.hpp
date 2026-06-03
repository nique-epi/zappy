/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ServerConfig — parsed command-line configuration of the Zappy server
*/

#pragma once

#include <string>
#include <vector>

namespace zappy::server {

/**
 * @brief Command-line configuration of the server.
 *
 * Mirrors the mandatory options: -p port, -x width, -y height,
 * -n team names, -c clients per team, -f frequency (reciprocal time unit).
 */
struct ServerConfig {
  int port{4242};
  int width{10};
  int height{10};
  int clientsPerTeam{1};
  int frequency{100};
  std::vector<std::string> teamNames;
};

}  // namespace zappy::server
