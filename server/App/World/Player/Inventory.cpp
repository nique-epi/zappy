/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Inventory — per-resource stock carried by a single player
*/

#include "App/World/Player/Inventory.hpp"

namespace zappy::world {

std::size_t Inventory::quantityOf(ResourceType type) const {
  return counts_[static_cast<std::size_t>(type)];
}

void Inventory::add(ResourceType type, std::size_t amount) {
  counts_[static_cast<std::size_t>(type)] += amount;
}

bool Inventory::consume(ResourceType type, std::size_t amount) {
  const std::size_t index = static_cast<std::size_t>(type);
  if (counts_[index] < amount) {
    return false;
  }
  counts_[index] -= amount;
  return true;
}

}  // namespace zappy::world
