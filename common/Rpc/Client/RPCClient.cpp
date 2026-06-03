/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** RPCClient — TCP client facade over Connection + ClientLoop
*/

#include "Rpc/Client/RPCClient.hpp"
#include <unistd.h>
#include <utility>

namespace zappy::rpc {

RPCClient::RPCClient(const std::string &ipAddress, int port)
    : connection_(std::in_place, ipAddress, port),
      loop_(STDIN_FILENO, connection_->fd(), realIo_) {
  loop_.setResponseHandler(
      [this](const std::string &line) { dispatcher_(line); });
}

RPCClient::RPCClient(const std::string &ipAddress, int port, bool pollStdin)
    : connection_(std::in_place, ipAddress, port),
      loop_(pollStdin ? STDIN_FILENO : -1, connection_->fd(), realIo_) {
  loop_.setResponseHandler(
      [this](const std::string &line) { dispatcher_(line); });
}

RPCClient::RPCClient(int stdinFd, int serverFd, network::ISocket &io)
    : connection_(std::nullopt), loop_(stdinFd, serverFd, io) {
  loop_.setResponseHandler(
      [this](const std::string &line) { dispatcher_(line); });
}

RPCClient &RPCClient::on(const RPCMessage &message,
                         ResponseDispatcher::Handler handler) {
  dispatcher_.on(message, std::move(handler));
  return *this;
}

RPCClient &RPCClient::on(const std::string &opcode,
                         ResponseDispatcher::Handler handler) {
  dispatcher_.on(opcode, std::move(handler));
  return *this;
}

RPCClient &RPCClient::onUnknown(ResponseDispatcher::Handler handler) {
  dispatcher_.onUnknown(std::move(handler));
  return *this;
}

RPCClient &RPCClient::onStdin(
    std::function<void(const std::string &)> handler) {
  loop_.setCommandHandler(std::move(handler));
  return *this;
}

void RPCClient::send(const std::string &line) { loop_.sendLine(line); }

void RPCClient::run() { loop_.run(); }

void RPCClient::runOnce(int timeoutMs) { loop_.runOnce(timeoutMs); }

void RPCClient::stop() { loop_.stop(); }

}  // namespace zappy::rpc
