/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Integration tests for installAiActionPipeline
*/

#include <gtest/gtest.h>
#include <chrono>
#include <string>
#include <vector>
#include "App/Scheduler/Scheduler.hpp"
#include "App/TimeUnit.hpp"
#include "LoopbackClient.hpp"
#include "Net/ActionQueue.hpp"
#include "Net/AiActionPipeline.hpp"
#include "Net/ClientContext.hpp"
#include "Rpc/Message/IMessage.hpp"
#include "Rpc/Server/RPCServer.hpp"
#include "Rpc/Session/Session.hpp"

using zappy::rpc::IMessage;
using zappy::rpc::RPCServer;
using zappy::rpc::Session;
using zappy::rpc::test::LoopbackClient;
using zappy::server::ClientContext;
using zappy::server::ClientType;
using zappy::server::installAiActionPipeline;
using zappy::server::maxPendingActions;
using zappy::server::Scheduler;
using zappy::server::TimeUnit;

namespace {

constexpr int driveIterations = 4;
constexpr int driveTimeoutMs = 20;
constexpr int testFrequency = 1000;
constexpr auto pastDeadline = std::chrono::seconds{10};

struct ServerHarness {
  RPCServer<ClientContext> server;
  Scheduler scheduler;
  TimeUnit timeUnit{testFrequency};
  std::vector<std::pair<int, std::string>> dispatches;

  ServerHarness() : server(0), scheduler() {
    server.onHandshake(
        [](Session<ClientContext>& session, const std::string& teamName) {
          session.ctx().type = ClientType::Ai;
          session.ctx().teamName = teamName;
          session.completeHandshake();
        });
    server.on(std::string("Forward"),
              [this](Session<ClientContext>& session, IMessage&) {
                dispatches.emplace_back(session.fd(), std::string("Forward"));
                session.send("ok");
              });
    server.on(std::string("Connect_nbr"),
              [this](Session<ClientContext>& session, IMessage&) {
                dispatches.emplace_back(session.fd(),
                                        std::string("Connect_nbr"));
                session.send("1");
              });
    installAiActionPipeline(server, scheduler, timeUnit);
    server.start();
  }

  void drive(int iterations = driveIterations) {
    for (int index = 0; index < iterations; index++) {
      server.runOnce(driveTimeoutMs);
    }
  }

  void runDueNow() { scheduler.runDue(Scheduler::Clock::now() + pastDeadline); }
};

void completeHandshake(LoopbackClient& client) { client.sendLine("team1"); }

}  // namespace

TEST(AiActionPipeline, FirstCommandSchedulesButDoesNotDispatchImmediately) {
  /*
   * Given an authenticated AI client
   * When it sends one Forward
   * Then the dispatcher is not invoked until the scheduler fires the deadline
   */
  ServerHarness harness;
  LoopbackClient client(harness.server.port());
  harness.drive();
  completeHandshake(client);
  harness.drive();

  client.sendLine("Forward");
  harness.drive();
  EXPECT_TRUE(harness.dispatches.empty());

  harness.runDueNow();
  ASSERT_EQ(harness.dispatches.size(), 1U);
  EXPECT_EQ(harness.dispatches[0].second, "Forward");
}

TEST(AiActionPipeline, CommandsDispatchInFifoOrderOnePerDeadline) {
  /*
   * Given three queued Forward commands from one client
   * When the scheduler is drained three times
   * Then exactly one command is dispatched per drain in arrival order
   */
  ServerHarness harness;
  LoopbackClient client(harness.server.port());
  harness.drive();
  completeHandshake(client);
  harness.drive();

  client.sendLine("Forward");
  client.sendLine("Forward");
  client.sendLine("Forward");
  harness.drive();

  harness.runDueNow();
  EXPECT_EQ(harness.dispatches.size(), 1U);
  harness.runDueNow();
  EXPECT_EQ(harness.dispatches.size(), 2U);
  harness.runDueNow();
  EXPECT_EQ(harness.dispatches.size(), 3U);
}

TEST(AiActionPipeline, OneClientDoesNotBlockAnother) {
  /*
   * Given two AI clients each with a pending Forward
   * When the scheduler is drained once
   * Then both clients see their Forward dispatched in the same drain
   */
  ServerHarness harness;
  LoopbackClient first(harness.server.port());
  harness.drive();
  LoopbackClient second(harness.server.port());
  harness.drive();
  completeHandshake(first);
  completeHandshake(second);
  harness.drive();

  first.sendLine("Forward");
  second.sendLine("Forward");
  harness.drive();

  harness.runDueNow();
  ASSERT_EQ(harness.dispatches.size(), 2U);
  EXPECT_NE(harness.dispatches[0].first, harness.dispatches[1].first);
}

TEST(AiActionPipeline, EleventhCommandIsDroppedSilently) {
  /*
   * Given a saturated queue (ten pending commands)
   * When an eleventh command is sent
   * Then only the first ten are dispatched after draining the scheduler
   */
  ServerHarness harness;
  LoopbackClient client(harness.server.port());
  harness.drive();
  completeHandshake(client);
  harness.drive();

  for (std::size_t index = 0; index < maxPendingActions + 1; index++) {
    client.sendLine("Forward");
  }
  harness.drive();

  for (std::size_t index = 0; index < maxPendingActions + 1; index++) {
    harness.runDueNow();
  }
  EXPECT_EQ(harness.dispatches.size(), maxPendingActions);
}

TEST(AiActionPipeline, ZeroCostCommandStillGoesThroughScheduler) {
  /*
   * Given a Connect_nbr command (cost 0)
   * When it is sent and the scheduler is drained
   * Then it dispatches on the first drain, never inline
   */
  ServerHarness harness;
  LoopbackClient client(harness.server.port());
  harness.drive();
  completeHandshake(client);
  harness.drive();

  client.sendLine("Connect_nbr");
  harness.drive();
  EXPECT_TRUE(harness.dispatches.empty());

  harness.runDueNow();
  ASSERT_EQ(harness.dispatches.size(), 1U);
  EXPECT_EQ(harness.dispatches[0].second, "Connect_nbr");
}

TEST(AiActionPipeline, DisconnectMidActionDoesNotCrash) {
  /*
   * Given an AI client with a pending action scheduled
   * When the client disconnects before the deadline fires
   * Then draining the scheduler is a no-op and the server stays healthy
   */
  ServerHarness harness;
  {
    LoopbackClient client(harness.server.port());
    harness.drive();
    completeHandshake(client);
    harness.drive();
    client.sendLine("Forward");
    harness.drive();
  }
  harness.drive();
  harness.runDueNow();
  EXPECT_TRUE(harness.dispatches.empty());
}
