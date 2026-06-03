/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for zappy::network::Client — pure buffer behaviour, no I/O
*/

#include <gtest/gtest.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <utility>
#include "Network/Server/Client.hpp"

using zappy::network::Client;

namespace {

int makeSocketPair(int &peer) {
  int pair[2];
  socketpair(AF_UNIX, SOCK_STREAM, 0, pair);
  peer = pair[1];
  return pair[0];
}

}  // namespace

TEST(Client, FdAccessible) {
  int peer = -1;
  int fd = makeSocketPair(peer);
  Client client(fd);
  EXPECT_EQ(client.fd(), fd);
  close(peer);
}

TEST(Client, MoveTransfersOwnership) {
  int peer = -1;
  int fd = makeSocketPair(peer);
  Client source(fd);
  Client moved(std::move(source));
  EXPECT_EQ(moved.fd(), fd);
  EXPECT_EQ(source.fd(), -1);
  close(peer);
}

TEST(Client, MoveAssignTransfersOwnership) {
  int firstPeer = -1;
  int secondPeer = -1;
  int firstFd = makeSocketPair(firstPeer);
  int secondFd = makeSocketPair(secondPeer);
  Client source(firstFd);
  Client target(secondFd);
  target = std::move(source);
  EXPECT_EQ(target.fd(), firstFd);
  EXPECT_EQ(source.fd(), -1);
  close(firstPeer);
  close(secondPeer);
}

TEST(Client, NoPendingOutputInitially) {
  int peer = -1;
  int fd = makeSocketPair(peer);
  Client client(fd);
  EXPECT_FALSE(client.hasPendingOutput());
  close(peer);
}

TEST(Client, AppendOutputSetsPending) {
  int peer = -1;
  int fd = makeSocketPair(peer);
  Client client(fd);
  client.appendOutput("hello\n");
  EXPECT_TRUE(client.hasPendingOutput());
  close(peer);
}

TEST(Client, PeekOutputReturnsBuffer) {
  int peer = -1;
  int fd = makeSocketPair(peer);
  Client client(fd);
  client.appendOutput("hello\n");
  EXPECT_EQ(client.peekOutput(), "hello\n");
  close(peer);
}

TEST(Client, ConsumeOutputTrimsBuffer) {
  int peer = -1;
  int fd = makeSocketPair(peer);
  Client client(fd);
  client.appendOutput("hello\n");
  client.consumeOutput(3);
  EXPECT_EQ(client.peekOutput(), "lo\n");
  close(peer);
}

TEST(Client, ConsumeOutputClearsPending) {
  int peer = -1;
  int fd = makeSocketPair(peer);
  Client client(fd);
  client.appendOutput("hi\n");
  client.consumeOutput(3);
  EXPECT_FALSE(client.hasPendingOutput());
  close(peer);
}

TEST(Client, ConsumeLineReturnsFalseWithoutNewline) {
  int peer = -1;
  int fd = makeSocketPair(peer);
  Client client(fd);
  client.appendInput("hello", 5);
  std::string line;
  EXPECT_FALSE(client.consumeLine(line));
  close(peer);
}

TEST(Client, ConsumeLineReturnsTrueOnNewline) {
  int peer = -1;
  int fd = makeSocketPair(peer);
  Client client(fd);
  client.appendInput("hello\n", 6);
  std::string line;
  EXPECT_TRUE(client.consumeLine(line));
  EXPECT_EQ(line, "hello");
  close(peer);
}

TEST(Client, ConsumeLineMultiple) {
  int peer = -1;
  int fd = makeSocketPair(peer);
  Client client(fd);
  client.appendInput("foo\nbar\n", 8);
  std::string line;
  EXPECT_TRUE(client.consumeLine(line));
  EXPECT_EQ(line, "foo");
  EXPECT_TRUE(client.consumeLine(line));
  EXPECT_EQ(line, "bar");
  EXPECT_FALSE(client.consumeLine(line));
  close(peer);
}

TEST(Client, ConsumeLineKeepsPartialRemainder) {
  int peer = -1;
  int fd = makeSocketPair(peer);
  Client client(fd);
  client.appendInput("foo\npar", 7);
  std::string line;
  EXPECT_TRUE(client.consumeLine(line));
  EXPECT_EQ(line, "foo");
  EXPECT_FALSE(client.consumeLine(line));
  client.appendInput("tial\n", 5);
  EXPECT_TRUE(client.consumeLine(line));
  EXPECT_EQ(line, "partial");
  close(peer);
}

TEST(Client, ConsumeLineEmptyLine) {
  int peer = -1;
  int fd = makeSocketPair(peer);
  Client client(fd);
  client.appendInput("\n", 1);
  std::string line;
  EXPECT_TRUE(client.consumeLine(line));
  EXPECT_EQ(line, "");
  close(peer);
}
