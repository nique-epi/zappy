/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for the zappy::network exception hierarchy
*/

#include <gtest/gtest.h>
#include <stdexcept>
#include <string>
#include "Network/Exceptions.hpp"

using zappy::network::AcceptError;
using zappy::network::BindError;
using zappy::network::ConnectError;
using zappy::network::ListenError;
using zappy::network::NetworkError;
using zappy::network::PollError;
using zappy::network::SocketError;

TEST(NetworkExceptions, MessageIsPreserved) {
  EXPECT_EQ(std::string(NetworkError("root").what()), "root");
  EXPECT_EQ(std::string(SocketError("socket").what()), "socket");
  EXPECT_EQ(std::string(BindError("bind").what()), "bind");
  EXPECT_EQ(std::string(ListenError("listen").what()), "listen");
  EXPECT_EQ(std::string(ConnectError("connect").what()), "connect");
  EXPECT_EQ(std::string(AcceptError("accept").what()), "accept");
  EXPECT_EQ(std::string(PollError("poll").what()), "poll");
}

TEST(NetworkExceptions, EverySubclassIsANetworkError) {
  EXPECT_THROW(throw SocketError("x"), NetworkError);
  EXPECT_THROW(throw BindError("x"), NetworkError);
  EXPECT_THROW(throw ListenError("x"), NetworkError);
  EXPECT_THROW(throw ConnectError("x"), NetworkError);
  EXPECT_THROW(throw AcceptError("x"), NetworkError);
  EXPECT_THROW(throw PollError("x"), NetworkError);
}

TEST(NetworkExceptions, NetworkErrorIsAStdRuntimeError) {
  EXPECT_THROW(throw BindError("x"), std::runtime_error);
}
