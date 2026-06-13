/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ServerHandshake — handshake state machine for the Zappy server protocol
*/

#include "Network/ServerHandshake.hpp"
#include <chrono>
#include <string>
#include "Exceptions/HandshakeException.hpp"

namespace zappy::gui {

namespace {
constexpr const char* welcomeMessage = "WELCOME";
constexpr const char* graphicTeamName = "GRAPHIC";
}  // namespace

ServerHandshake::ServerHandshake(INetworkClient& network, int timeoutMs)
    : network_(network),
      deadline_(std::chrono::steady_clock::now() +
                std::chrono::milliseconds(timeoutMs)) {
  network_.setResponseHandler(
      [this](const std::string& line) { onServerLine(line); });
}

HandshakeStatus ServerHandshake::status() const { return status_; }

void ServerHandshake::checkTimeout() const {
  if (status_ == HandshakeStatus::Done) {
    return;
  }
  if (std::chrono::steady_clock::now() >= deadline_) {
    throw HandshakeTimeoutException();
  }
}

void ServerHandshake::onServerLine(const std::string& line) {
  if (status_ != HandshakeStatus::Pending || line != welcomeMessage) {
    return;
  }
  network_.sendLine(graphicTeamName);
  status_ = HandshakeStatus::Done;
}

}  // namespace zappy::gui
