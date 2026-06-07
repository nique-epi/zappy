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

constexpr int defaultPort = 4242;
constexpr int defaultWidth = 10;
constexpr int defaultHeight = 10;
constexpr int defaultClientsPerTeam = 1;
constexpr int defaultFrequency = 100;

/**
 * @brief Command-line configuration of the server.
 *
 * Mirrors the mandatory options: -p port, -x width, -y height,
 * -n team names, -c clients per team, -f frequency (reciprocal time unit).
 */
struct ServerConfig {
  int port{defaultPort};
  int width{defaultWidth};
  int height{defaultHeight};
  int clientsPerTeam{defaultClientsPerTeam};
  int frequency{defaultFrequency};
  std::vector<std::string> teamNames;
};

}  // namespace zappy::server
