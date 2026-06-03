/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** RAII TCP socket wrapper (shared between server and client)
*/

#include "Network/Socket/TcpSocket.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include "Network/Exceptions.hpp"

namespace zappy::network {

TcpSocket::TcpSocket() {
  fd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (fd_ < 0) {
    throw SocketError("socket() failed");
  }
}

TcpSocket::~TcpSocket() {
  if (fd_ >= 0) {
    close(fd_);
  }
}

TcpSocket::TcpSocket(TcpSocket &&other) noexcept : fd_(other.fd_) {
  other.fd_ = -1;
}

TcpSocket &TcpSocket::operator=(TcpSocket &&other) noexcept {
  if (this != &other) {
    if (fd_ >= 0) {
      close(fd_);
    }
    fd_ = other.fd_;
    other.fd_ = -1;
  }
  return *this;
}

void TcpSocket::setReuseAddress() {
  int option = 1;
  if (setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) < 0) {
    throw SocketError("setsockopt(SO_REUSEADDR) failed");
  }
}

int TcpSocket::fd() const { return fd_; }

}  // namespace zappy::network
