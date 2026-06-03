/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** AiProtocol — message structs, schemas and definitions for the AI protocol
*/

#include "Protocol/AiProtocol.hpp"
#include <memory>
#include "Schema/Fields/Fields.hpp"

namespace zappy::protocol::ai {

using zappy::rpc::RPCMessage;
using zappy::rpc::TypedMessage;
using zappy::schema::LongStringFieldType;
using zappy::schema::Schema;
using zappy::schema::StringFieldType;

namespace {

std::shared_ptr<const Schema<ObjectArgs>> objectSchema() {
  static const auto schema = std::make_shared<const Schema<ObjectArgs>>(
      Schema<ObjectArgs>{}.field<StringFieldType>("object",
                                                  &ObjectArgs::object));
  return schema;
}

std::shared_ptr<const Schema<BroadcastTextArgs>> broadcastTextSchema() {
  static const auto schema = std::make_shared<const Schema<BroadcastTextArgs>>(
      Schema<BroadcastTextArgs>{}.field<LongStringFieldType>(
          "text", &BroadcastTextArgs::text));
  return schema;
}

}  // namespace

const TypedMessage<ObjectArgs> &Take() {
  static const auto message = RPCMessage{"Take"}.schema(objectSchema());
  return message;
}

const TypedMessage<ObjectArgs> &Set() {
  static const auto message = RPCMessage{"Set"}.schema(objectSchema());
  return message;
}

const TypedMessage<BroadcastTextArgs> &Broadcast() {
  static const auto message =
      RPCMessage{"Broadcast"}.schema(broadcastTextSchema());
  return message;
}

const RPCMessage &Forward() {
  static const RPCMessage message{"Forward"};
  return message;
}

const RPCMessage &Right() {
  static const RPCMessage message{"Right"};
  return message;
}

const RPCMessage &Left() {
  static const RPCMessage message{"Left"};
  return message;
}

const RPCMessage &Look() {
  static const RPCMessage message{"Look"};
  return message;
}

const RPCMessage &Inventory() {
  static const RPCMessage message{"Inventory"};
  return message;
}

const RPCMessage &ConnectNbr() {
  static const RPCMessage message{"Connect_nbr"};
  return message;
}

const RPCMessage &Fork() {
  static const RPCMessage message{"Fork"};
  return message;
}

const RPCMessage &Eject() {
  static const RPCMessage message{"Eject"};
  return message;
}

const RPCMessage &Incantation() {
  static const RPCMessage message{"Incantation"};
  return message;
}

const RPCMessage &Ok() {
  static const RPCMessage message{"ok"};
  return message;
}

const RPCMessage &Ko() {
  static const RPCMessage message{"ko"};
  return message;
}

const RPCMessage &Dead() {
  static const RPCMessage message{"dead"};
  return message;
}

}  // namespace zappy::protocol::ai
