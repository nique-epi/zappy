/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** RAII TCP connection to a remote server (client side)
*/

#include "Network/Client/Connection.hpp"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include "Network/Exceptions.hpp"

namespace zappy::network {

Connection::Connection(const std::string &ipAddress, int port) {
  fd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (fd_ < 0) {
    throw ConnectError("socket() failed: " + std::string(strerror(errno)));
  }
  struct sockaddr_in address = {};
  address.sin_family = AF_INET;
  address.sin_port = htons(static_cast<uint16_t>(port));
  if (inet_pton(AF_INET, ipAddress.c_str(), &address.sin_addr) != 1) {
    close(fd_);
    throw ConnectError("invalid IP address: " + ipAddress);
  }
  if (connect(fd_, reinterpret_cast<struct sockaddr *>(&address),
              sizeof(address)) < 0) {
    close(fd_);
    throw ConnectError("connect() failed: " + std::string(strerror(errno)));
  }
  int flag = 1;
  setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));
}

Connection::~Connection() {
  if (fd_ >= 0) {
    close(fd_);
  }
}

Connection::Connection(Connection &&other) noexcept : fd_(other.fd_) {
  other.fd_ = -1;
}

Connection &Connection::operator=(Connection &&other) noexcept {
  if (this != &other) {
    if (fd_ >= 0) {
      close(fd_);
    }
    fd_ = other.fd_;
    other.fd_ = -1;
  }
  return *this;
}

int Connection::fd() const { return fd_; }

}  // namespace zappy::network
