/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** WorldState
*/

#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace zappy::gui {

constexpr std::size_t RESOURCE_COUNT = 7;
constexpr std::size_t EGG_ALIGN = 16;

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
};

struct alignas(EGG_ALIGN) Egg {
  int id{};
  int playerId{};
  int x{};
  int y{};
};

struct WorldState {
  int width{};
  int height{};
  std::vector<std::vector<Tile>> tiles;
  std::vector<Player> players;
  std::vector<Egg> eggs;
  std::vector<std::string> teams;
};

WorldState mockWorld();

}  // namespace zappy::gui
