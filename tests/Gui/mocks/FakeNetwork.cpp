/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** FakeNetwork — in-memory INetworkClient test double
*/

#include "FakeNetwork.hpp"
#include <utility>

namespace zappy::gui::test {

void FakeNetwork::setResponseHandler(ResponseHandler handler) {
  handler_ = std::move(handler);
}

void FakeNetwork::sendLine(const std::string& line) { sent_.push_back(line); }

void FakeNetwork::inject(const std::string& line) {
  if (handler_) {
    handler_(line);
  }
}

const std::vector<std::string>& FakeNetwork::sent() const { return sent_; }

}  // namespace zappy::gui::test
