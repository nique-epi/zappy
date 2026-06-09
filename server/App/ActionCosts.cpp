/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ActionCosts — game-time cost of every AI protocol opcode
*/

#include "App/ActionCosts.hpp"
#include <array>
#include <utility>

namespace zappy::server {

namespace {

struct OpcodeCost {
  std::string_view opcode;
  int cost;
};

constexpr std::array<OpcodeCost, 12> opcodeCostTable{{
    {.opcode = "Forward", .cost = 7},
    {.opcode = "Right", .cost = 7},
    {.opcode = "Left", .cost = 7},
    {.opcode = "Look", .cost = 7},
    {.opcode = "Inventory", .cost = 1},
    {.opcode = "Broadcast", .cost = 7},
    {.opcode = "Connect_nbr", .cost = 0},
    {.opcode = "Fork", .cost = 42},
    {.opcode = "Eject", .cost = 7},
    {.opcode = "Take", .cost = 7},
    {.opcode = "Set", .cost = 7},
    {.opcode = "Incantation", .cost = 300},
}};

}  // namespace

int costOf(std::string_view opcode) {
  for (const OpcodeCost& entry : opcodeCostTable) {
    if (entry.opcode == opcode) {
      return entry.cost;
    }
  }
  return defaultActionCost;
}

}  // namespace zappy::server
