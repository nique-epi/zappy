/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for zappy::network::PollLoop — deterministic via MockSocket
*/

#include <gtest/gtest.h>
#include <cerrno>
#include <string>
#include <vector>
#include "MockSocket.hpp"
#include "Network/Exceptions.hpp"
#include "Network/Server/Client.hpp"
#include "Network/Server/PollLoop.hpp"

using zappy::network::AcceptError;
using zappy::network::Client;
using zappy::network::PollLoop;
using zappy::network::test::MockSocket;

namespace {
constexpr int serverFd = 10;
constexpr int firstClientFd = 42;
constexpr int secondClientFd = 43;
}  // namespace

TEST(PollLoop, ClientCountStartsAtZero) {
  MockSocket mock;
  PollLoop loop(serverFd, mock);
  EXPECT_EQ(loop.clientCount(), 0U);
}

TEST(PollLoop, NewConnectionIncreasesClientCount) {
  MockSocket mock;
  mock.pushPollIn(serverFd);
  mock.pushAccept(firstClientFd);

  PollLoop loop(serverFd, mock);
  loop.runOnce();

  EXPECT_EQ(loop.clientCount(), 1U);
}

TEST(PollLoop, HandlerCalledOnLine) {
  MockSocket mock;
  std::string received;

  mock.pushPollIn(serverFd);
  mock.pushAccept(firstClientFd);
  mock.pushPollIn(firstClientFd);
  mock.pushReceive(firstClientFd, "HELLO\n");

  PollLoop loop(serverFd, mock);
  loop.setHandler([&](Client &, const std::string &line) { received = line; });

  loop.runOnce();
  loop.runOnce();

  EXPECT_EQ(received, "HELLO");
}

TEST(PollLoop, MultipleLinesInOneReadDispatchedInOrder) {
  MockSocket mock;
  std::vector<std::string> received;

  mock.pushPollIn(serverFd);
  mock.pushAccept(firstClientFd);
  mock.pushPollIn(firstClientFd);
  mock.pushReceive(firstClientFd, "A\nB\nC\n");

  PollLoop loop(serverFd, mock);
  loop.setHandler(
      [&](Client &, const std::string &line) { received.push_back(line); });

  loop.runOnce();
  loop.runOnce();

  ASSERT_EQ(received.size(), 3U);
  EXPECT_EQ(received[0], "A");
  EXPECT_EQ(received[1], "B");
  EXPECT_EQ(received[2], "C");
}

TEST(PollLoop, PollOutOnlyWhenBuffered) {
  MockSocket mock;

  mock.pushPollIn(serverFd);
  mock.pushAccept(firstClientFd);
  mock.pushPollIn(firstClientFd);
  mock.pushReceive(firstClientFd, "PING\n");
  mock.pushPollOut(firstClientFd);

  PollLoop loop(serverFd, mock);
  loop.setHandler([&](Client &client, const std::string &) {
    loop.sendTo(client.fd(), "PONG\n");
  });

  loop.runOnce();
  loop.runOnce();
  loop.runOnce();

  EXPECT_EQ(mock.sentTo(firstClientFd), "PONG\n");
}

TEST(PollLoop, BroadcastReachesEveryClient) {
  MockSocket mock;

  mock.pushPollIn(serverFd);
  mock.pushAccept(firstClientFd);
  mock.pushPollIn(serverFd);
  mock.pushAccept(secondClientFd);
  mock.pushPoll({{firstClientFd, POLLOUT}, {secondClientFd, POLLOUT}});

  PollLoop loop(serverFd, mock);
  loop.runOnce();
  loop.runOnce();
  loop.broadcast("EVENT\n");
  loop.runOnce();

  EXPECT_EQ(mock.sentTo(firstClientFd), "EVENT\n");
  EXPECT_EQ(mock.sentTo(secondClientFd), "EVENT\n");
}

TEST(PollLoop, PartialSendKeepsRemainderBuffered) {
  MockSocket mock;

  mock.pushPollIn(serverFd);
  mock.pushAccept(firstClientFd);

  PollLoop loop(serverFd, mock);
  loop.runOnce();
  loop.sendTo(firstClientFd, "HELLO\n");
  mock.pushPollOut(firstClientFd);
  loop.runOnce();

  EXPECT_EQ(mock.sentTo(firstClientFd), "HELLO\n");
}

TEST(PollLoop, DisconnectRemovesClient) {
  MockSocket mock;

  mock.pushPollIn(serverFd);
  mock.pushAccept(firstClientFd);
  mock.pushPollIn(firstClientFd);
  mock.pushDisconnect(firstClientFd);

  PollLoop loop(serverFd, mock);
  loop.runOnce();
  EXPECT_EQ(loop.clientCount(), 1U);
  loop.runOnce();
  EXPECT_EQ(loop.clientCount(), 0U);
}

TEST(PollLoop, MultipleClientsHandledIndependently) {
  MockSocket mock;
  std::string fromFirst;
  std::string fromSecond;

  mock.pushPollIn(serverFd);
  mock.pushAccept(firstClientFd);
  mock.pushPollIn(serverFd);
  mock.pushAccept(secondClientFd);
  mock.pushPollIn(firstClientFd);
  mock.pushReceive(firstClientFd, "A\n");
  mock.pushPollIn(secondClientFd);
  mock.pushReceive(secondClientFd, "B\n");

  PollLoop loop(serverFd, mock);
  loop.setHandler([&](Client &client, const std::string &line) {
    if (client.fd() == firstClientFd) {
      fromFirst = line;
    }
    if (client.fd() == secondClientFd) {
      fromSecond = line;
    }
  });

  loop.runOnce();
  loop.runOnce();
  loop.runOnce();
  loop.runOnce();

  EXPECT_EQ(fromFirst, "A");
  EXPECT_EQ(fromSecond, "B");
}

TEST(PollLoop, AcceptErrorThrows) {
  MockSocket mock;
  mock.pushPollIn(serverFd);

  PollLoop loop(serverFd, mock);
  EXPECT_THROW(loop.runOnce(), AcceptError);
}

TEST(PollLoop, ConnectHandlerCalledOnAccept) {
  MockSocket mock;
  mock.pushPollIn(serverFd);
  mock.pushAccept(firstClientFd);

  int connectedFd = -1;
  PollLoop loop(serverFd, mock);
  loop.setConnectHandler([&](int fd) { connectedFd = fd; });
  loop.runOnce();

  EXPECT_EQ(connectedFd, firstClientFd);
}

TEST(PollLoop, DisconnectHandlerCalledOnDisconnect) {
  MockSocket mock;
  mock.pushPollIn(serverFd);
  mock.pushAccept(firstClientFd);
  mock.pushPollIn(firstClientFd);
  mock.pushDisconnect(firstClientFd);

  int disconnectedFd = -1;
  PollLoop loop(serverFd, mock);
  loop.setDisconnectHandler([&](int fd) { disconnectedFd = fd; });
  loop.runOnce();
  loop.runOnce();

  EXPECT_EQ(disconnectedFd, firstClientFd);
}

TEST(PollLoop, TimeoutWithNoEventsIsNoOp) {
  MockSocket mock;
  PollLoop loop(serverFd, mock);
  loop.runOnce(0);
  EXPECT_EQ(loop.clientCount(), 0U);
}

TEST(PollLoop, SendToUnknownClientIsIgnored) {
  MockSocket mock;
  PollLoop loop(serverFd, mock);
  loop.sendTo(firstClientFd, "ignored\n");
  EXPECT_EQ(mock.sentTo(firstClientFd), "");
}

TEST(PollLoop, PollErrorThrowsPollError) {
  MockSocket mock;
  mock.pushPollError(EBADF);

  PollLoop loop(serverFd, mock);
  EXPECT_THROW(loop.runOnce(), zappy::network::PollError);
}

TEST(PollLoop, PollInterruptedBySignalIsIgnored) {
  MockSocket mock;
  mock.pushPollError(EINTR);

  PollLoop loop(serverFd, mock);
  EXPECT_NO_THROW(loop.runOnce());
  EXPECT_EQ(loop.clientCount(), 0U);
}

TEST(PollLoop, PollHupRemovesClient) {
  MockSocket mock;
  mock.pushPollIn(serverFd);
  mock.pushAccept(firstClientFd);
  mock.pushPoll({{firstClientFd, POLLHUP}});

  int disconnectedFd = -1;
  PollLoop loop(serverFd, mock);
  loop.setDisconnectHandler([&](int fd) { disconnectedFd = fd; });
  loop.runOnce();
  EXPECT_EQ(loop.clientCount(), 1U);
  loop.runOnce();

  EXPECT_EQ(loop.clientCount(), 0U);
  EXPECT_EQ(disconnectedFd, firstClientFd);
}

TEST(PollLoop, RunStopsWhenStopCalledFromHandler) {
  MockSocket mock;
  mock.pushPollIn(serverFd);
  mock.pushAccept(firstClientFd);

  PollLoop loop(serverFd, mock);
  loop.setConnectHandler([&](int) { loop.stop(); });
  loop.run();

  EXPECT_EQ(loop.clientCount(), 1U);
}

TEST(PollLoop, SendFailureRemovesClient) {
  MockSocket mock;
  mock.pushPollIn(serverFd);
  mock.pushAccept(firstClientFd);

  PollLoop loop(serverFd, mock);
  loop.runOnce();
  loop.sendTo(firstClientFd, "data\n");
  mock.failNextSend(firstClientFd);
  mock.pushPollOut(firstClientFd);
  loop.runOnce();

  EXPECT_EQ(loop.clientCount(), 0U);
}
