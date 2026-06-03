/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Production I/O backend — wraps real syscalls
*/

#pragma once

#include "Network/Socket/ISocket.hpp"

namespace zappy::network {

/**
 * @brief Production ISocket implementation — thin wrappers around POSIX
 *        syscalls.
 */
class RealSocket : public ISocket {
 public:
  int doPoll(struct pollfd *fds, nfds_t nfds, int timeout) override;
  int doAccept(int fd, struct sockaddr *address, socklen_t *length) override;
  ssize_t doReceive(int fd, void *buffer, size_t length, int flags) override;
  ssize_t doSend(int fd, const void *buffer, size_t length, int flags) override;
};

}  // namespace zappy::network
