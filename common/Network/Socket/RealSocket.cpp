/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Production I/O backend — wraps real syscalls
*/

#include "Network/Socket/RealSocket.hpp"
#include <unistd.h>
#include <cerrno>

namespace zappy::network {

int RealSocket::doPoll(struct pollfd *fds, nfds_t nfds, int timeout) {
  return poll(fds, nfds, timeout);
}

int RealSocket::doAccept(int fd, struct sockaddr *address, socklen_t *length) {
  return accept(fd, address, length);
}

ssize_t RealSocket::doReceive(int fd, void *buffer, size_t length, int flags) {
  ssize_t received = recv(fd, buffer, length, flags);
  if (received < 0 && errno == ENOTSOCK) {
    return ::read(fd, buffer, length);
  }
  return received;
}

ssize_t RealSocket::doSend(int fd, const void *buffer, size_t length,
                           int flags) {
  return send(fd, buffer, length, flags);
}

}  // namespace zappy::network
