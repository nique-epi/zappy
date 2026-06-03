/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Integration tests for zappy::rpc::RPCServer over a real loopback socket
*/

#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <utility>
#include "LoopbackClient.hpp"
#include "Network/Exceptions.hpp"
#include "Rpc/Message/IMessage.hpp"
#include "Rpc/Message/RPCMessage.hpp"
#include "Rpc/Server/RPCServer.hpp"
#include "Rpc/Session/Session.hpp"
#include "Schema/Fields/NumberFieldType.hpp"
#include "Schema/Fields/StringFieldType.hpp"
#include "Schema/Schema.hpp"

using zappy::network::BindError;
using zappy::network::NetworkError;
using zappy::rpc::IMessage;
using zappy::rpc::RPCMessage;
using zappy::rpc::RPCServer;
using zappy::rpc::Session;
using zappy::rpc::test::LoopbackClient;

namespace {

struct NoContext {};

struct TeamContext {
  std::string team;
  int messageCount = 0;
};

struct EchoArgs {
  std::string text;
};

struct NumberArgs {
  std::string value;
};

template <typename Ctx>
void drive(RPCServer<Ctx> &server, int iterations) {
  for (int index = 0; index < iterations; index++) {
    server.runOnce(100);
  }
}

std::shared_ptr<const zappy::schema::Schema<EchoArgs>> makeEchoSchema() {
  zappy::schema::Schema<EchoArgs> schema;
  schema.field<zappy::schema::StringFieldType>("text", &EchoArgs::text);
  return std::make_shared<const zappy::schema::Schema<EchoArgs>>(
      std::move(schema));
}

std::shared_ptr<const zappy::schema::Schema<NumberArgs>> makeNumberSchema() {
  zappy::schema::Schema<NumberArgs> schema;
  schema.field<zappy::schema::NumberFieldType>("value", &NumberArgs::value);
  return std::make_shared<const zappy::schema::Schema<NumberArgs>>(
      std::move(schema));
}

}  // namespace

// ---------------------------------------------------------------------------
// Bind / listen lifecycle
// ---------------------------------------------------------------------------

TEST(RPCServer, StartAssignsConcretePortWhenZero) {
  RPCServer<NoContext> server(0);
  server.start();
  EXPECT_GT(server.port(), 0);
}

TEST(RPCServer, StartBindsAndAcceptsConnection) {
  RPCServer<NoContext> server(0);
  server.start();
  EXPECT_NO_THROW(LoopbackClient client(server.port()));
}

TEST(RPCServer, DoubleStartThrowsBindError) {
  RPCServer<NoContext> server(0);
  server.start();
  EXPECT_THROW(server.start(), BindError);
}

TEST(RPCServer, TwoServersOnSamePortThrowsBindError) {
  RPCServer<NoContext> first(0);
  first.start();
  RPCServer<NoContext> second(first.port());
  EXPECT_THROW(second.start(), BindError);
}

TEST(RPCServer, BindErrorIsNetworkError) {
  RPCServer<NoContext> first(0);
  first.start();
  RPCServer<NoContext> second(first.port());
  EXPECT_THROW(second.start(), NetworkError);
}

// ---------------------------------------------------------------------------
// Connection lifecycle handlers
// ---------------------------------------------------------------------------

TEST(RPCServer, ConnectHandlerFiresOnAccept) {
  RPCServer<NoContext> server(0);
  int connectCount = 0;
  server.onConnect([&](Session<NoContext> &) { connectCount++; });
  server.start();

  LoopbackClient client(server.port());
  drive(server, 1);

  EXPECT_EQ(connectCount, 1);
}

TEST(RPCServer, DisconnectHandlerFiresOnClose) {
  RPCServer<NoContext> server(0);
  int disconnectCount = 0;
  server.onDisconnect([&](Session<NoContext> &) { disconnectCount++; });
  server.start();

  {
    LoopbackClient client(server.port());
    drive(server, 1);
  }
  drive(server, 1);

  EXPECT_EQ(disconnectCount, 1);
}

// ---------------------------------------------------------------------------
// Dispatch
// ---------------------------------------------------------------------------

TEST(RPCServer, TypedHandlerReceivesParsedArguments) {
  auto echoSchema = makeEchoSchema();
  const auto echo = RPCMessage{"ech"}.schema(echoSchema);

  RPCServer<NoContext> server(0);
  server.on(echo, [](Session<NoContext> &session, const EchoArgs &args) {
    session.send("ech " + args.text);
  });
  server.start();

  LoopbackClient client(server.port());
  drive(server, 1);
  client.sendLine("ech hello");
  drive(server, 2);

  EXPECT_EQ(client.receive(), "ech hello\n");
}

TEST(RPCServer, RawOpcodeHandlerInvoked) {
  RPCServer<NoContext> server(0);
  server.on(std::string("ping"), [](Session<NoContext> &session, IMessage &) {
    session.send("pong");
  });
  server.start();

  LoopbackClient client(server.port());
  drive(server, 1);
  client.sendLine("ping");
  drive(server, 2);

  EXPECT_EQ(client.receive(), "pong\n");
}

TEST(RPCServer, UnknownOpcodeRoutesToUnknownHandler) {
  RPCServer<NoContext> server(0);
  server.onUnknown(
      [](Session<NoContext> &session, IMessage &) { session.send("unknown"); });
  server.start();

  LoopbackClient client(server.port());
  drive(server, 1);
  client.sendLine("whatisthis");
  drive(server, 2);

  EXPECT_EQ(client.receive(), "unknown\n");
}

TEST(RPCServer, SchemaFailureRepliesSbpByDefault) {
  auto numberSchema = makeNumberSchema();
  const auto number = RPCMessage{"num"}.schema(numberSchema);

  RPCServer<NoContext> server(0);
  server.on(number, [](Session<NoContext> &, const NumberArgs &) {});
  server.start();

  LoopbackClient client(server.port());
  drive(server, 1);
  client.sendLine("num notanumber");
  drive(server, 2);

  EXPECT_EQ(client.receive(), "sbp\n");
}

TEST(RPCServer, SchemaFailureRoutesToOnInvalidWhenSet) {
  auto numberSchema = makeNumberSchema();
  const auto number = RPCMessage{"num"}.schema(numberSchema);

  RPCServer<NoContext> server(0);
  server.on(number, [](Session<NoContext> &, const NumberArgs &) {});
  server.onInvalid(
      [](Session<NoContext> &session, IMessage &) { session.send("ko"); });
  server.start();

  LoopbackClient client(server.port());
  drive(server, 1);
  client.sendLine("num notanumber");
  drive(server, 2);

  EXPECT_EQ(client.receive(), "ko\n");
}

// ---------------------------------------------------------------------------
// Handshake
// ---------------------------------------------------------------------------

TEST(RPCServer, HandshakeInterceptsLinesUntilCompleted) {
  auto echoSchema = makeEchoSchema();
  const auto echo = RPCMessage{"ech"}.schema(echoSchema);

  RPCServer<TeamContext> server(0);
  server.onHandshake(
      [](Session<TeamContext> &session, const std::string &line) {
        session.ctx().team = line;
        session.send("welcome");
        session.completeHandshake();
      });
  server.on(echo, [](Session<TeamContext> &session, const EchoArgs &args) {
    session.send("ech " + args.text);
  });
  server.start();

  LoopbackClient client(server.port());
  drive(server, 1);
  client.sendLine("TEAM1");
  drive(server, 2);
  EXPECT_EQ(client.receive(), "welcome\n");

  client.sendLine("ech now");
  drive(server, 2);
  EXPECT_EQ(client.receive(), "ech now\n");
}

// ---------------------------------------------------------------------------
// Context + broadcast
// ---------------------------------------------------------------------------

TEST(RPCServer, ContextPersistsAcrossMessages) {
  auto echoSchema = makeEchoSchema();
  const auto echo = RPCMessage{"ech"}.schema(echoSchema);

  RPCServer<TeamContext> server(0);
  server.on(echo, [](Session<TeamContext> &session, const EchoArgs &) {
    session.ctx().messageCount++;
    session.send("count " + std::to_string(session.ctx().messageCount));
  });
  server.start();

  LoopbackClient client(server.port());
  drive(server, 1);
  client.sendLine("ech a");
  drive(server, 2);
  EXPECT_EQ(client.receive(), "count 1\n");
  client.sendLine("ech b");
  drive(server, 2);
  EXPECT_EQ(client.receive(), "count 2\n");
}

TEST(RPCServer, BroadcastReachesEveryClient) {
  RPCServer<NoContext> server(0);
  server.start();

  LoopbackClient first(server.port());
  drive(server, 1);
  LoopbackClient second(server.port());
  drive(server, 1);

  server.broadcast(std::string("event"));
  drive(server, 1);

  EXPECT_EQ(first.receive(), "event\n");
  EXPECT_EQ(second.receive(), "event\n");
}

TEST(RPCServer, RespondBuildsWireLineFromArguments) {
  auto echoSchema = makeEchoSchema();
  const auto echo = RPCMessage{"ech"}.schema(echoSchema);

  RPCServer<NoContext> server(0);
  server.on(std::string("go"), [&](Session<NoContext> &session, IMessage &) {
    server.respond(session, echo, {"built"});
  });
  server.start();

  LoopbackClient client(server.port());
  drive(server, 1);
  client.sendLine("go");
  drive(server, 2);

  EXPECT_EQ(client.receive(), "ech built\n");
}

TEST(RPCServer, BroadcastBuildsWireLineFromArguments) {
  auto echoSchema = makeEchoSchema();
  const auto echo = RPCMessage{"ech"}.schema(echoSchema);

  RPCServer<NoContext> server(0);
  server.start();

  LoopbackClient client(server.port());
  drive(server, 1);
  server.broadcast(echo, {"hi"});
  drive(server, 1);

  EXPECT_EQ(client.receive(), "ech hi\n");
}

TEST(RPCServer, RawHandlerReadsMessageArguments) {
  RPCServer<NoContext> server(0);
  server.on(std::string("sum"),
            [](Session<NoContext> &session, IMessage &message) {
              session.send("argc " + std::to_string(message.argCount()) +
                           " first " + message.arg(0));
            });
  server.start();

  LoopbackClient client(server.port());
  drive(server, 1);
  client.sendLine("sum alpha beta");
  drive(server, 2);

  EXPECT_EQ(client.receive(), "argc 2 first alpha\n");
}

TEST(RPCServer, ForEachSessionVisitsEveryConnection) {
  RPCServer<NoContext> server(0);
  server.start();

  LoopbackClient first(server.port());
  drive(server, 1);
  LoopbackClient second(server.port());
  drive(server, 1);

  int sessionCount = 0;
  server.forEachSession([&](Session<NoContext> &) { sessionCount++; });

  EXPECT_EQ(sessionCount, 2);
}
