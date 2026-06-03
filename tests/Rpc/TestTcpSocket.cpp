/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for zappy::network::TcpSocket
*/

#include <gtest/gtest.h>
#include <sys/socket.h>
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
  char byte = 0;
  EXPECT_LT(recv(fd, &byte, 1, MSG_DONTWAIT), 0);
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
