/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ActionQueue — bounded FIFO of pending AI command lines for one session
*/

#include "Net/ActionQueue.hpp"
#include <utility>

namespace zappy::server {

bool ActionQueue::push(const std::string& line) {
  if (lines_.size() >= maxPendingActions) {
    return false;
  }
  lines_.push_back(line);
  return true;
}

std::optional<std::string> ActionQueue::pop() {
  if (lines_.empty()) {
    return std::nullopt;
  }
  std::string front = std::move(lines_.front());
  lines_.pop_front();
  return front;
}

bool ActionQueue::empty() const { return lines_.empty(); }

bool ActionQueue::full() const { return lines_.size() >= maxPendingActions; }

std::size_t ActionQueue::size() const { return lines_.size(); }

void ActionQueue::clear() { lines_.clear(); }

}  // namespace zappy::server
