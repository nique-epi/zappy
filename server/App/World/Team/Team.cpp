/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Team — a named group owning a pool of eggs (free slots)
*/

#include "App/World/Team/Team.hpp"
#include <stdexcept>
#include <utility>

namespace zappy::world {

Team::Team(std::string name) : name_(std::move(name)) {}

const std::string& Team::name() const { return name_; }

std::size_t Team::freeSlots() const { return eggs_.size(); }

bool Team::empty() const { return eggs_.empty(); }

const std::vector<Egg>& Team::eggs() const { return eggs_; }

void Team::addEgg(Egg egg) { eggs_.push_back(std::move(egg)); }

Egg Team::removeEggAt(std::size_t index) {
  if (index >= eggs_.size()) {
    throw std::out_of_range("Team::removeEggAt index past end");
  }
  Egg removed = eggs_[index];
  eggs_.erase(eggs_.begin() + static_cast<std::ptrdiff_t>(index));
  return removed;
}

bool Team::removeEggById(int eggId) {
  for (auto it = eggs_.begin(); it != eggs_.end(); ++it) {
    if (it->id == eggId) {
      eggs_.erase(it);
      return true;
    }
  }
  return false;
}

}  // namespace zappy::world
