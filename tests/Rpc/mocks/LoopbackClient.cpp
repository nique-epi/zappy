/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Real loopback TCP client used to drive RPCServer integration tests
*/

#include "LoopbackClient.hpp"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <cstdint>
#include <cstring>
#include <stdexcept>

namespace zappy::rpc::test {

namespace {
constexpr int receiveBufferSize = 4096;
constexpr int receiveTimeoutSeconds = 2;
}  // namespace

LoopbackClient::LoopbackClient(int port) {
  fd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (fd_ < 0) {
    throw std::runtime_error("LoopbackClient: socket() failed");
  }
  struct timeval timeout{};
  timeout.tv_sec = receiveTimeoutSeconds;
  setsockopt(fd_, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

  struct sockaddr_in address{};
  address.sin_family = AF_INET;
  address.sin_port = htons(static_cast<uint16_t>(port));
  inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);
  if (connect(fd_, reinterpret_cast<struct sockaddr *>(&address),
              sizeof(address)) < 0) {
    close(fd_);
    throw std::runtime_error("LoopbackClient: connect() failed");
  }
}

LoopbackClient::~LoopbackClient() {
  if (fd_ >= 0) {
    close(fd_);
  }
}

void LoopbackClient::sendLine(const std::string &line) {
  std::string payload = line + "\n";
  ssize_t sent = send(fd_, payload.data(), payload.size(), 0);
  (void)sent;
}

std::string LoopbackClient::receive() {
  char buffer[receiveBufferSize];
  ssize_t received = recv(fd_, buffer, sizeof(buffer), 0);
  if (received <= 0) {
    return {};
  }
  return std::string(buffer, static_cast<std::size_t>(received));
}

int LoopbackClient::fd() const { return fd_; }

}  // namespace zappy::rpc::test
