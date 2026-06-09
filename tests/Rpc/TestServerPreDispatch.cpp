/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Tests for RPCServer::onAuthenticatedLine pre-dispatch hook
*/

#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "LoopbackClient.hpp"
#include "Rpc/Message/IMessage.hpp"
#include "Rpc/Server/RPCServer.hpp"
#include "Rpc/Session/Session.hpp"

using zappy::rpc::IMessage;
using zappy::rpc::RPCServer;
using zappy::rpc::Session;
using zappy::rpc::test::LoopbackClient;

namespace {

struct NoContext {};

template <typename Ctx>
void drive(RPCServer<Ctx>& server, int iterations) {
  for (int index = 0; index < iterations; index++) {
    server.runOnce(100);
  }
}

}  // namespace

TEST(RPCServerPreDispatch, HookInterceptsAuthenticatedLines) {
  /*
   * Given a server with an onAuthenticatedLine hook that drops every line
   * When a client sends a line that would normally route to a handler
   * Then the dispatcher handler is not invoked
   */
  RPCServer<NoContext> server(0);
  bool handlerCalled = false;
  std::vector<std::string> seenLines;
  server.on(std::string("ping"),
            [&](Session<NoContext>&, IMessage&) { handlerCalled = true; });
  server.onAuthenticatedLine([&](Session<NoContext>&, const std::string& line) {
    seenLines.push_back(line);
  });
  server.start();

  LoopbackClient client(server.port());
  drive(server, 1);
  client.sendLine("ping");
  drive(server, 2);

  EXPECT_FALSE(handlerCalled);
  ASSERT_EQ(seenLines.size(), 1U);
  EXPECT_EQ(seenLines[0], "ping");
}

TEST(RPCServerPreDispatch, HookCanForwardThroughDispatchLine) {
  /*
   * Given a hook that forwards every line via dispatchLine
   * When a client sends a known opcode
   * Then the registered handler still receives it
   */
  RPCServer<NoContext> server(0);
  server.on(std::string("ping"), [&](Session<NoContext>& session, IMessage&) {
    session.send("pong");
  });
  server.onAuthenticatedLine(
      [&](Session<NoContext>& session, const std::string& line) {
        server.dispatchLine(session, line);
      });
  server.start();

  LoopbackClient client(server.port());
  drive(server, 1);
  client.sendLine("ping");
  drive(server, 2);

  EXPECT_EQ(client.receive(), "pong\n");
}

TEST(RPCServerPreDispatch, HookDoesNotInterceptHandshakeLines) {
  /*
   * Given a server with both a handshake handler and an authenticated-line
   *   hook
   * When the first line of a connection arrives
   * Then only the handshake handler runs and the hook is bypassed
   */
  RPCServer<NoContext> server(0);
  bool hookCalled = false;
  bool handshakeCalled = false;
  server.onHandshake([&](Session<NoContext>& session, const std::string& line) {
    handshakeCalled = true;
    session.completeHandshake();
    session.send("hello " + line);
  });
  server.onAuthenticatedLine(
      [&](Session<NoContext>&, const std::string&) { hookCalled = true; });
  server.start();

  LoopbackClient client(server.port());
  drive(server, 1);
  client.sendLine("TEAM");
  drive(server, 2);

  EXPECT_TRUE(handshakeCalled);
  EXPECT_FALSE(hookCalled);
  EXPECT_EQ(client.receive(), "hello TEAM\n");
}
