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

const zappy::rpc::TypedMessage<ObjectArgs> &Take();
const zappy::rpc::TypedMessage<ObjectArgs> &Set();
const zappy::rpc::TypedMessage<BroadcastTextArgs> &Broadcast();

const zappy::rpc::RPCMessage &Forward();
const zappy::rpc::RPCMessage &Right();
const zappy::rpc::RPCMessage &Left();
const zappy::rpc::RPCMessage &Look();
const zappy::rpc::RPCMessage &Inventory();
const zappy::rpc::RPCMessage &ConnectNbr();
const zappy::rpc::RPCMessage &Fork();
const zappy::rpc::RPCMessage &Eject();
const zappy::rpc::RPCMessage &Incantation();

const zappy::rpc::RPCMessage &Ok();
const zappy::rpc::RPCMessage &Ko();
const zappy::rpc::RPCMessage &Dead();

}  // namespace zappy::protocol::ai
