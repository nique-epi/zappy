/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for zappy::rpc::ResponseDispatcher
*/

#include <gtest/gtest.h>
#include <string>
#include "Rpc/Client/ResponseDispatcher.hpp"
#include "Rpc/Message/IMessage.hpp"
#include "Rpc/Message/RPCMessage.hpp"
#include "Schema/Fields/NumberFieldType.hpp"
#include "Schema/Fields/StringFieldType.hpp"
#include "Schema/Schema.hpp"

using zappy::rpc::IMessage;
using zappy::rpc::ResponseDispatcher;
using zappy::rpc::RPCMessage;

namespace {

struct WelcomeArgs {
  std::string name;
};

struct CountArgs {
  std::string value;
};

zappy::schema::Schema<WelcomeArgs> makeWelcomeSchema() {
  zappy::schema::Schema<WelcomeArgs> schema;
  schema.field<zappy::schema::StringFieldType>("name", &WelcomeArgs::name);
  return schema;
}

zappy::schema::Schema<CountArgs> makeCountSchema() {
  zappy::schema::Schema<CountArgs> schema;
  schema.field<zappy::schema::NumberFieldType>("value", &CountArgs::value);
  return schema;
}

}  // namespace

TEST(ResponseDispatcher, RawOpcodeHandlerInvoked) {
  ResponseDispatcher dispatcher;
  std::string seen;
  dispatcher.on(std::string("pong"),
                [&](IMessage &message) { seen = message.opcode(); });

  dispatcher("pong");

  EXPECT_EQ(seen, "pong");
}

TEST(ResponseDispatcher, TypedHandlerReceivesParsedValue) {
  auto schema = makeWelcomeSchema();
  const auto welcome = RPCMessage{"wlc"}.schema(schema);

  ResponseDispatcher dispatcher;
  std::string name;
  dispatcher.on(welcome, [&](const WelcomeArgs &args) { name = args.name; });

  dispatcher("wlc alice");

  EXPECT_EQ(name, "alice");
}

TEST(ResponseDispatcher, TypedHandlerSkippedOnSchemaFailure) {
  auto schema = makeCountSchema();
  const auto count = RPCMessage{"cnt"}.schema(schema);

  ResponseDispatcher dispatcher;
  bool called = false;
  dispatcher.on(count, [&](const CountArgs &) { called = true; });

  dispatcher("cnt notanumber");

  EXPECT_FALSE(called);
}

TEST(ResponseDispatcher, UnknownOpcodeRoutesToUnknownHandler) {
  ResponseDispatcher dispatcher;
  std::string seen;
  dispatcher.onUnknown([&](IMessage &message) { seen = message.opcode(); });

  dispatcher("mystery a b");

  EXPECT_EQ(seen, "mystery");
}

TEST(ResponseDispatcher, BlankLineIsIgnored) {
  ResponseDispatcher dispatcher;
  bool called = false;
  dispatcher.onUnknown([&](IMessage &) { called = true; });

  dispatcher("");

  EXPECT_FALSE(called);
}

TEST(ResponseDispatcher, RpcMessageRegistrationRoutesByOpcode) {
  ResponseDispatcher dispatcher;
  std::string seen;
  const RPCMessage ready{"rdy"};
  dispatcher.on(ready, [&](IMessage &message) { seen = message.opcode(); });

  dispatcher("rdy");

  EXPECT_EQ(seen, "rdy");
}
