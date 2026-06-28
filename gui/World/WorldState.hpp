/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** WorldState
*/

#pragma once

#include <array>
#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace zappy::gui {

constexpr std::size_t RESOURCE_COUNT = 7;
constexpr std::size_t EGG_ALIGN = 16;
constexpr int MAX_PLAYER_LEVEL = 8;
constexpr int PLAYERS_REQUIRED_FOR_VICTORY = 6;

enum class Orientation : std::uint8_t {
  North = 1,
  East = 2,
  South = 3,
  West = 4
};

struct Tile {
  int x{};
  int y{};
  std::array<int, RESOURCE_COUNT> resources{};
};

struct Player {
  int id{};
  int x{};
  int y{};
  Orientation orientation{Orientation::North};
  int level{};
  std::string teamName;
  std::array<int, RESOURCE_COUNT> inventory{};
  bool alive{true};
};

struct alignas(EGG_ALIGN) Egg {
  int id{};
  int playerId{};
  int x{};
  int y{};
};

struct Incantation {
  int x{};
  int y{};
  int level{};
};

struct IncantationEnd {
  int x{};
  int y{};
  bool success{};
  std::chrono::steady_clock::time_point endTime{};
};

struct PlayerEjection {
  int playerId{};
  std::chrono::steady_clock::time_point startTime{};
};

struct PlayerBroadcast {
  int playerId{};
  std::chrono::steady_clock::time_point startTime{};
};

struct WorldState {
  int width{};
  int height{};
  std::vector<std::vector<Tile>> tiles;
  std::vector<Player> players;
  std::vector<Egg> eggs;
  std::vector<std::string> teams;
  int timeUnit{};
  bool gameOver{};
  std::string winnerTeam;
  std::vector<Incantation> activeIncantations;
  std::vector<IncantationEnd> finishedIncantations;
  std::vector<PlayerEjection> playerEjections;
  std::vector<PlayerBroadcast> playerBroadcasts;
};

WorldState mockWorld();

}  // namespace zappy::gui
