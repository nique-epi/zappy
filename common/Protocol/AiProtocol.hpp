/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** AiProtocol — message structs, schemas and definitions for the AI protocol
*/

#pragma once

#include <string>
#include "Rpc/Message/RPCMessage.hpp"
#include "Rpc/Message/TypedMessage.hpp"
#include "Schema/Schema.hpp"

namespace zappy::protocol::ai {

/**
 * @brief Object name carried by Take/Set (Take <object>, Set <object>).
 */
struct ObjectArgs {
  std::string object;
};

/**
 * @brief Free-form broadcast text (Broadcast <text>).
 */
struct BroadcastTextArgs {
  std::string text;
};

extern const zappy::schema::Schema<ObjectArgs> ObjectSchema;
extern const zappy::schema::Schema<BroadcastTextArgs> BroadcastTextSchema;

extern const zappy::rpc::TypedMessage<ObjectArgs> Take;
extern const zappy::rpc::TypedMessage<ObjectArgs> Set;
extern const zappy::rpc::TypedMessage<BroadcastTextArgs> Broadcast;

extern const zappy::rpc::RPCMessage Forward;
extern const zappy::rpc::RPCMessage Right;
extern const zappy::rpc::RPCMessage Left;
extern const zappy::rpc::RPCMessage Look;
extern const zappy::rpc::RPCMessage Inventory;
extern const zappy::rpc::RPCMessage ConnectNbr;
extern const zappy::rpc::RPCMessage Fork;
extern const zappy::rpc::RPCMessage Eject;
extern const zappy::rpc::RPCMessage Incantation;

extern const zappy::rpc::RPCMessage Ok;
extern const zappy::rpc::RPCMessage Ko;
extern const zappy::rpc::RPCMessage Dead;

}  // namespace zappy::protocol::ai
