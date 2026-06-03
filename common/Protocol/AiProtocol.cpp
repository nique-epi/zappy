/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** AiProtocol — message structs, schemas and definitions for the AI protocol
*/

#include "Protocol/AiProtocol.hpp"
#include "Schema/Fields/Fields.hpp"

namespace zappy::protocol::ai {

using zappy::rpc::RPCMessage;
using zappy::schema::LongStringFieldType;
using zappy::schema::Schema;
using zappy::schema::StringFieldType;

const Schema<ObjectArgs> ObjectSchema =
    Schema<ObjectArgs>{}.field<StringFieldType>("object", &ObjectArgs::object);

const Schema<BroadcastTextArgs> BroadcastTextSchema =
    Schema<BroadcastTextArgs>{}.field<LongStringFieldType>(
        "text", &BroadcastTextArgs::text);

const zappy::rpc::TypedMessage<ObjectArgs> Take =
    RPCMessage{"Take"}.schema(ObjectSchema);
const zappy::rpc::TypedMessage<ObjectArgs> Set =
    RPCMessage{"Set"}.schema(ObjectSchema);
const zappy::rpc::TypedMessage<BroadcastTextArgs> Broadcast =
    RPCMessage{"Broadcast"}.schema(BroadcastTextSchema);

const RPCMessage Forward{"Forward"};
const RPCMessage Right{"Right"};
const RPCMessage Left{"Left"};
const RPCMessage Look{"Look"};
const RPCMessage Inventory{"Inventory"};
const RPCMessage ConnectNbr{"Connect_nbr"};
const RPCMessage Fork{"Fork"};
const RPCMessage Eject{"Eject"};
const RPCMessage Incantation{"Incantation"};

const RPCMessage Ok{"ok"};
const RPCMessage Ko{"ko"};
const RPCMessage Dead{"dead"};

}  // namespace zappy::protocol::ai
