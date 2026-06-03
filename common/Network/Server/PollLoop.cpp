/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** poll() event loop — accept, read, write, with injectable wake-up timeout
*/

#include "Network/Server/PollLoop.hpp"
#include <netinet/in.h>
#include <sys/socket.h>
#include <cerrno>
#include "Network/Exceptions.hpp"

namespace zappy::network {

namespace {
constexpr std::size_t readBufferSize = 4096;
}

PollLoop::PollLoop(int serverFd, ISocket &io)
    : serverFd_(serverFd), io_(io), running_(false), dirty_(true) {}

void PollLoop::setHandler(CommandHandler handler) {
  handler_ = std::move(handler);
}

void PollLoop::setConnectHandler(ConnectHandler handler) {
  connectHandler_ = std::move(handler);
}

void PollLoop::setDisconnectHandler(DisconnectHandler handler) {
  disconnectHandler_ = std::move(handler);
}

void PollLoop::stop() { running_ = false; }

void PollLoop::run(int timeoutMs) {
  running_ = true;
  while (running_) {
    runOnce(timeoutMs);
  }
}

void PollLoop::runOnce(int timeoutMs) {
  if (dirty_) {
    rebuildFds();
    dirty_ = false;
  }
  int ready =
      io_.doPoll(fds_.data(), static_cast<nfds_t>(fds_.size()), timeoutMs);
  if (ready < 0) {
    if (errno == EINTR) {
      return;
    }
    throw PollError("poll() failed");
  }

  if (fds_[0].revents & POLLIN) {
    handleNewConnection();
  }

  for (std::size_t i = 1; i < fds_.size(); i++) {
    int fd = fds_[i].fd;
    short revents = fds_[i].revents;

    if (revents & (POLLERR | POLLHUP | POLLNVAL)) {
      removeClient(fd);
      dirty_ = true;
      break;
    }
    if (revents & POLLIN) {
      handleClientRead(fd);
      if (clients_.find(fd) == clients_.end()) {
        dirty_ = true;
        break;
      }
    }
    if (revents & POLLOUT) {
      handleClientWrite(fd);
    }
  }
}

void PollLoop::sendTo(int clientFd, const std::string &data) {
  auto it = clients_.find(clientFd);
  if (it == clients_.end()) {
    return;
  }
  it->second.appendOutput(data);
  dirty_ = true;
}

void PollLoop::broadcast(const std::string &data) {
  for (auto &[fd, client] : clients_) {
    client.appendOutput(data);
  }
  if (!clients_.empty()) {
    dirty_ = true;
  }
}

std::size_t PollLoop::clientCount() const { return clients_.size(); }

void PollLoop::handleNewConnection() {
  struct sockaddr_in address = {};
  socklen_t length = sizeof(address);
  int clientFd = io_.doAccept(
      serverFd_, reinterpret_cast<struct sockaddr *>(&address), &length);
  if (clientFd < 0) {
    throw AcceptError("accept() failed");
  }
  clients_.emplace(clientFd, Client(clientFd));
  dirty_ = true;
  if (connectHandler_) {
    connectHandler_(clientFd);
  }
}

void PollLoop::handleClientRead(int fd) {
  char buffer[readBufferSize];
  ssize_t received = io_.doReceive(fd, buffer, sizeof(buffer), 0);
  if (received <= 0) {
    removeClient(fd);
    return;
  }
  Client &client = clients_.at(fd);
  client.appendInput(buffer, static_cast<std::size_t>(received));

  std::string line;
  while (client.consumeLine(line)) {
    if (handler_) {
      handler_(client, line);
    }
  }
}

void PollLoop::handleClientWrite(int fd) {
  auto it = clients_.find(fd);
  if (it == clients_.end()) {
    return;
  }
  Client &client = it->second;
  const std::string &data = client.peekOutput();
  ssize_t sent = io_.doSend(fd, data.c_str(), data.size(), MSG_NOSIGNAL);
  if (sent < 0) {
    removeClient(fd);
    dirty_ = true;
    return;
  }
  client.consumeOutput(static_cast<std::size_t>(sent));
  if (!client.hasPendingOutput()) {
    dirty_ = true;
  }
}

void PollLoop::removeClient(int fd) {
  clients_.erase(fd);
  if (disconnectHandler_) {
    disconnectHandler_(fd);
  }
}

void PollLoop::rebuildFds() {
  fds_.clear();
  fds_.push_back({serverFd_, POLLIN, 0});
  for (auto &[fd, client] : clients_) {
    short events = POLLIN;
    if (client.hasPendingOutput()) {
      events |= POLLOUT;
    }
    fds_.push_back({fd, events, 0});
  }
}

}  // namespace zappy::network
