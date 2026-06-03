/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for zappy::network::RealSocket — exercises the real syscalls
*/

#include <gtest/gtest.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include "Network/Socket/RealSocket.hpp"

using zappy::network::RealSocket;

TEST(RealSocket, ReceiveReadsFromConnectedSocket) {
  int pair[2];
  ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, pair), 0);
  const char *message = "ping";
  ASSERT_EQ(send(pair[0], message, 4, 0), 4);

  RealSocket io;
  char buffer[8] = {};
  ssize_t received = io.doReceive(pair[1], buffer, sizeof(buffer), 0);

  EXPECT_EQ(received, 4);
  EXPECT_EQ(std::string(buffer, 4), "ping");
  close(pair[0]);
  close(pair[1]);
}

TEST(RealSocket, ReceiveFallsBackToReadOnNonSocketFd) {
  int pipeFds[2];
  ASSERT_EQ(pipe(pipeFds), 0);
  const char *message = "data";
  ASSERT_EQ(write(pipeFds[1], message, 4), 4);

  RealSocket io;
  char buffer[8] = {};
  ssize_t received = io.doReceive(pipeFds[0], buffer, sizeof(buffer), 0);

  EXPECT_EQ(received, 4);
  EXPECT_EQ(std::string(buffer, 4), "data");
  close(pipeFds[0]);
  close(pipeFds[1]);
}

TEST(RealSocket, SendWritesToConnectedSocket) {
  int pair[2];
  ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, pair), 0);

  RealSocket io;
  ssize_t sent = io.doSend(pair[0], "hi", 2, 0);

  EXPECT_EQ(sent, 2);
  close(pair[0]);
  close(pair[1]);
}

TEST(RealSocket, AcceptOnNonListeningSocketFails) {
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  ASSERT_GE(fd, 0);

  RealSocket io;
  EXPECT_LT(io.doAccept(fd, nullptr, nullptr), 0);
  close(fd);
}

TEST(RealSocket, PollReportsReadableSocket) {
  int pair[2];
  ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, pair), 0);
  ASSERT_EQ(send(pair[0], "x", 1, 0), 1);

  RealSocket io;
  struct pollfd fds[1];
  fds[0].fd = pair[1];
  fds[0].events = POLLIN;
  fds[0].revents = 0;
  int ready = io.doPoll(fds, 1, 100);

  EXPECT_GE(ready, 1);
  EXPECT_TRUE(fds[0].revents & POLLIN);
  close(pair[0]);
  close(pair[1]);
}
