/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Player — a single drone in the world (position, direction, level, inventory)
*/

#include "App/World/Player/Player.hpp"
#include <utility>

namespace zappy::world {

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
Player::Player(int id, std::string teamName, int x, int y, Direction direction)
    : id_(id),
      teamName_(std::move(teamName)),
      x_(x),
      y_(y),
      direction_(direction) {
  inventory_.add(ResourceType::Food, initialFoodUnits);
}

int Player::id() const { return id_; }
const std::string& Player::teamName() const { return teamName_; }
int Player::x() const { return x_; }
int Player::y() const { return y_; }
Direction Player::direction() const { return direction_; }
int Player::level() const { return level_; }

const Inventory& Player::inventory() const { return inventory_; }
Inventory& Player::inventory() { return inventory_; }

void Player::setPosition(int x, int y) {
  x_ = x;
  y_ = y;
}

void Player::setDirection(Direction direction) { direction_ = direction; }
void Player::setLevel(int level) { level_ = level; }

}  // namespace zappy::world
