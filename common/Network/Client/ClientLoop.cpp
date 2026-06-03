/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Client poll() event loop — multiplexes stdin and the server socket
*/

#include "Network/Client/ClientLoop.hpp"
#include <poll.h>
#include <sys/socket.h>
#include <cerrno>
#include <utility>
#include "Network/Exceptions.hpp"

namespace zappy::network {

namespace {
constexpr std::size_t readBufferSize = 4096;
}

ClientLoop::ClientLoop(int stdinFd, int serverFd, ISocket &io)
    : stdinFd_(stdinFd), serverFd_(serverFd), io_(io), running_(false) {}

void ClientLoop::setCommandHandler(CommandHandler handler) {
  commandHandler_ = std::move(handler);
}

void ClientLoop::setResponseHandler(ResponseHandler handler) {
  responseHandler_ = std::move(handler);
}

void ClientLoop::sendLine(const std::string &line) {
  outputBuffer_ += line + "\n";
}

void ClientLoop::stop() { running_ = false; }

void ClientLoop::run() {
  running_ = true;
  while (running_) {
    runOnce(-1);
  }
}

void ClientLoop::runOnce(int timeoutMs) {
  short serverEvents = POLLIN;
  if (!outputBuffer_.empty()) {
    serverEvents |= POLLOUT;
  }

  struct pollfd fds[2];
  nfds_t count = 0;
  nfds_t stdinIndex = 0;
  if (stdinFd_ >= 0) {
    fds[count] = {stdinFd_, POLLIN, 0};
    stdinIndex = count;
    count++;
  }
  nfds_t serverIndex = count;
  fds[count] = {serverFd_, serverEvents, 0};
  count++;

  int ready = io_.doPoll(fds, count, timeoutMs);
  if (ready < 0) {
    if (errno == EINTR) {
      return;
    }
    throw PollError("poll() failed");
  }

  if (stdinFd_ >= 0 && (fds[stdinIndex].revents & POLLIN)) {
    handleStdin();
  }
  if (fds[serverIndex].revents & (POLLERR | POLLHUP | POLLNVAL)) {
    running_ = false;
    return;
  }
  if (fds[serverIndex].revents & POLLIN) {
    handleServerRead();
  }
  if (fds[serverIndex].revents & POLLOUT) {
    flushOutput();
  }
}

void ClientLoop::handleStdin() {
  char buffer[readBufferSize];
  ssize_t received = io_.doReceive(stdinFd_, buffer, sizeof(buffer), 0);
  if (received <= 0) {
    running_ = false;
    return;
  }
  stdinBuffer_.append(buffer, static_cast<std::size_t>(received));
  std::string::size_type position;
  while ((position = stdinBuffer_.find('\n')) != std::string::npos) {
    std::string line = stdinBuffer_.substr(0, position);
    stdinBuffer_.erase(0, position + 1);
    if (commandHandler_) {
      commandHandler_(line);
    }
  }
}

void ClientLoop::handleServerRead() {
  char buffer[readBufferSize];
  ssize_t received = io_.doReceive(serverFd_, buffer, sizeof(buffer), 0);
  if (received <= 0) {
    running_ = false;
    return;
  }
  serverBuffer_.append(buffer, static_cast<std::size_t>(received));
  std::string::size_type position;
  while ((position = serverBuffer_.find('\n')) != std::string::npos) {
    std::string line = serverBuffer_.substr(0, position);
    serverBuffer_.erase(0, position + 1);
    if (responseHandler_) {
      responseHandler_(line);
    }
  }
}

void ClientLoop::flushOutput() {
  ssize_t sent = io_.doSend(serverFd_, outputBuffer_.c_str(),
                            outputBuffer_.size(), MSG_NOSIGNAL);
  if (sent > 0) {
    outputBuffer_.erase(0, static_cast<std::size_t>(sent));
  }
}

}  // namespace zappy::network
