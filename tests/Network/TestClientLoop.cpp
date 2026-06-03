/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for zappy::network::ClientLoop — deterministic via MockSocket
*/

#include <gtest/gtest.h>
#include <cerrno>
#include <string>
#include <vector>
#include "MockSocket.hpp"
#include "Network/Client/ClientLoop.hpp"
#include "Network/Exceptions.hpp"

using zappy::network::ClientLoop;
using zappy::network::test::MockSocket;

namespace {
constexpr int stdinFd = 10;
constexpr int serverFd = 3;
}  // namespace

TEST(ClientLoop, ResponseHandlerCalledOnServerLine) {
  MockSocket mock;
  std::string received;
  mock.pushPollIn(serverFd);
  mock.pushReceive(serverFd, "HELLO\n");

  ClientLoop loop(stdinFd, serverFd, mock);
  loop.setResponseHandler([&](const std::string &line) { received = line; });
  loop.runOnce();

  EXPECT_EQ(received, "HELLO");
}

TEST(ClientLoop, MultipleServerLinesDispatchedInOrder) {
  MockSocket mock;
  std::vector<std::string> lines;
  mock.pushPollIn(serverFd);
  mock.pushReceive(serverFd, "A\nB\nC\n");

  ClientLoop loop(stdinFd, serverFd, mock);
  loop.setResponseHandler(
      [&](const std::string &line) { lines.push_back(line); });
  loop.runOnce();

  ASSERT_EQ(lines.size(), 3U);
  EXPECT_EQ(lines[0], "A");
  EXPECT_EQ(lines[1], "B");
  EXPECT_EQ(lines[2], "C");
}

TEST(ClientLoop, SendLineFlushesOnPollOut) {
  MockSocket mock;
  mock.pushPollOut(serverFd);

  ClientLoop loop(stdinFd, serverFd, mock);
  loop.sendLine("PING");
  loop.runOnce();

  EXPECT_EQ(mock.sentTo(serverFd), "PING\n");
}

TEST(ClientLoop, CommandHandlerCalledOnStdinLine) {
  MockSocket mock;
  std::string received;
  mock.pushPollIn(stdinFd);
  mock.pushReceive(stdinFd, "HELLO\n");

  ClientLoop loop(stdinFd, serverFd, mock);
  loop.setCommandHandler([&](const std::string &line) { received = line; });
  loop.runOnce();

  EXPECT_EQ(received, "HELLO");
}

TEST(ClientLoop, StdinDisabledPollsServerOnly) {
  MockSocket mock;
  std::string received;
  mock.pushPollIn(serverFd);
  mock.pushReceive(serverFd, "ONLY\n");

  ClientLoop loop(-1, serverFd, mock);
  loop.setResponseHandler([&](const std::string &line) { received = line; });
  loop.runOnce();

  EXPECT_EQ(received, "ONLY");
}

TEST(ClientLoop, ServerDisconnectDoesNotThrow) {
  MockSocket mock;
  mock.pushPollIn(serverFd);
  mock.pushDisconnect(serverFd);

  ClientLoop loop(stdinFd, serverFd, mock);
  EXPECT_NO_THROW(loop.runOnce());
}

TEST(ClientLoop, StdinEofDoesNotThrow) {
  MockSocket mock;
  mock.pushPollIn(stdinFd);
  mock.pushDisconnect(stdinFd);

  ClientLoop loop(stdinFd, serverFd, mock);
  EXPECT_NO_THROW(loop.runOnce());
}

TEST(ClientLoop, PollErrorThrowsPollError) {
  MockSocket mock;
  mock.pushPollError(EBADF);

  ClientLoop loop(stdinFd, serverFd, mock);
  EXPECT_THROW(loop.runOnce(), zappy::network::PollError);
}

TEST(ClientLoop, PollInterruptedBySignalIsIgnored) {
  MockSocket mock;
  mock.pushPollError(EINTR);

  ClientLoop loop(stdinFd, serverFd, mock);
  EXPECT_NO_THROW(loop.runOnce());
}

TEST(ClientLoop, RunStopsOnServerDisconnect) {
  MockSocket mock;
  mock.pushPollIn(serverFd);
  mock.pushDisconnect(serverFd);

  ClientLoop loop(stdinFd, serverFd, mock);
  EXPECT_NO_THROW(loop.run());
}

TEST(ClientLoop, RunStopsWhenStopCalledFromHandler) {
  MockSocket mock;
  mock.pushPollIn(serverFd);
  mock.pushReceive(serverFd, "BYE\n");

  ClientLoop loop(stdinFd, serverFd, mock);
  loop.setResponseHandler([&](const std::string &) { loop.stop(); });
  EXPECT_NO_THROW(loop.run());
}
