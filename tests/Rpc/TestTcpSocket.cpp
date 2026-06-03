/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for zappy::network::TcpSocket
*/

#include <fcntl.h>
#include <gtest/gtest.h>
#include <cerrno>
#include <utility>
#include "Network/Socket/TcpSocket.hpp"

using zappy::network::TcpSocket;

TEST(TcpSocket, ConstructorCreatesValidFd) {
  TcpSocket socket;
  EXPECT_GE(socket.fd(), 0);
}

TEST(TcpSocket, DestructorClosesFd) {
  int fd = -1;
  {
    TcpSocket socket;
    fd = socket.fd();
  }
  EXPECT_EQ(fcntl(fd, F_GETFD), -1);
  EXPECT_EQ(errno, EBADF);
}

TEST(TcpSocket, MoveTransfersOwnership) {
  TcpSocket source;
  int fd = source.fd();
  TcpSocket moved(std::move(source));
  EXPECT_EQ(moved.fd(), fd);
  EXPECT_EQ(source.fd(), -1);
}

TEST(TcpSocket, MoveAssignTransfersOwnership) {
  TcpSocket source;
  int fd = source.fd();
  TcpSocket target;
  target = std::move(source);
  EXPECT_EQ(target.fd(), fd);
  EXPECT_EQ(source.fd(), -1);
}

TEST(TcpSocket, SetReuseAddressDoesNotThrow) {
  TcpSocket socket;
  EXPECT_NO_THROW(socket.setReuseAddress());
}
