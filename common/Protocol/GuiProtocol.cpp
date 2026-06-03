/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** GuiProtocol — message structs, schemas and definitions for the GUI protocol
*/

#include "Protocol/GuiProtocol.hpp"
#include "Schema/Fields/Fields.hpp"

namespace zappy::protocol {

using zappy::rpc::RPCMessage;
using zappy::schema::LongStringFieldType;
using zappy::schema::NumberFieldType;
using zappy::schema::Schema;
using zappy::schema::StringFieldType;

const Schema<MapSizeArgs> MapSizeSchema =
    Schema<MapSizeArgs>{}
        .field<NumberFieldType>("width", &MapSizeArgs::width)
        .field<NumberFieldType>("height", &MapSizeArgs::height);

const Schema<TileContentArgs> TileContentSchema =
    Schema<TileContentArgs>{}
        .field<NumberFieldType>("positionX", &TileContentArgs::positionX)
        .field<NumberFieldType>("positionY", &TileContentArgs::positionY)
        .field<NumberFieldType>("food", &TileContentArgs::food)
        .field<NumberFieldType>("linemate", &TileContentArgs::linemate)
        .field<NumberFieldType>("deraumere", &TileContentArgs::deraumere)
        .field<NumberFieldType>("sibur", &TileContentArgs::sibur)
        .field<NumberFieldType>("mendiane", &TileContentArgs::mendiane)
        .field<NumberFieldType>("phiras", &TileContentArgs::phiras)
        .field<NumberFieldType>("thystame", &TileContentArgs::thystame);

const Schema<TeamNameArgs> TeamNameSchema =
    Schema<TeamNameArgs>{}.field<StringFieldType>("teamName",
                                                  &TeamNameArgs::teamName);

const Schema<NewPlayerArgs> NewPlayerSchema =
    Schema<NewPlayerArgs>{}
        .field<NumberFieldType>("playerNumber", &NewPlayerArgs::playerNumber)
        .field<NumberFieldType>("positionX", &NewPlayerArgs::positionX)
        .field<NumberFieldType>("positionY", &NewPlayerArgs::positionY)
        .field<NumberFieldType>("orientation", &NewPlayerArgs::orientation)
        .field<NumberFieldType>("level", &NewPlayerArgs::level)
        .field<StringFieldType>("teamName", &NewPlayerArgs::teamName);

const Schema<PlayerPositionArgs> PlayerPositionSchema =
    Schema<PlayerPositionArgs>{}
        .field<NumberFieldType>("playerNumber",
                                &PlayerPositionArgs::playerNumber)
        .field<NumberFieldType>("positionX", &PlayerPositionArgs::positionX)
        .field<NumberFieldType>("positionY", &PlayerPositionArgs::positionY)
        .field<NumberFieldType>("orientation",
                                &PlayerPositionArgs::orientation);

const Schema<PlayerLevelArgs> PlayerLevelSchema =
    Schema<PlayerLevelArgs>{}
        .field<NumberFieldType>("playerNumber", &PlayerLevelArgs::playerNumber)
        .field<NumberFieldType>("level", &PlayerLevelArgs::level);

const Schema<PlayerInventoryArgs> PlayerInventorySchema =
    Schema<PlayerInventoryArgs>{}
        .field<NumberFieldType>("playerNumber",
                                &PlayerInventoryArgs::playerNumber)
        .field<NumberFieldType>("positionX", &PlayerInventoryArgs::positionX)
        .field<NumberFieldType>("positionY", &PlayerInventoryArgs::positionY)
        .field<NumberFieldType>("food", &PlayerInventoryArgs::food)
        .field<NumberFieldType>("linemate", &PlayerInventoryArgs::linemate)
        .field<NumberFieldType>("deraumere", &PlayerInventoryArgs::deraumere)
        .field<NumberFieldType>("sibur", &PlayerInventoryArgs::sibur)
        .field<NumberFieldType>("mendiane", &PlayerInventoryArgs::mendiane)
        .field<NumberFieldType>("phiras", &PlayerInventoryArgs::phiras)
        .field<NumberFieldType>("thystame", &PlayerInventoryArgs::thystame);

const Schema<ExpulsionArgs> ExpulsionSchema =
    Schema<ExpulsionArgs>{}.field<NumberFieldType>(
        "playerNumber", &ExpulsionArgs::playerNumber);

const Schema<BroadcastArgs> BroadcastSchema =
    Schema<BroadcastArgs>{}
        .field<NumberFieldType>("playerNumber", &BroadcastArgs::playerNumber)
        .field<LongStringFieldType>("message", &BroadcastArgs::message);

const Schema<IncantationEndArgs> IncantationEndSchema =
    Schema<IncantationEndArgs>{}
        .field<NumberFieldType>("positionX", &IncantationEndArgs::positionX)
        .field<NumberFieldType>("positionY", &IncantationEndArgs::positionY)
        .field<NumberFieldType>("result", &IncantationEndArgs::result);

const Schema<EggLayingArgs> EggLayingSchema =
    Schema<EggLayingArgs>{}.field<NumberFieldType>(
        "playerNumber", &EggLayingArgs::playerNumber);

const Schema<ResourceDropArgs> ResourceDropSchema =
    Schema<ResourceDropArgs>{}
        .field<NumberFieldType>("playerNumber", &ResourceDropArgs::playerNumber)
        .field<NumberFieldType>("resourceNumber",
                                &ResourceDropArgs::resourceNumber);

const Schema<ResourceCollectArgs> ResourceCollectSchema =
    Schema<ResourceCollectArgs>{}
        .field<NumberFieldType>("playerNumber",
                                &ResourceCollectArgs::playerNumber)
        .field<NumberFieldType>("resourceNumber",
                                &ResourceCollectArgs::resourceNumber);

const Schema<PlayerDeathArgs> PlayerDeathSchema =
    Schema<PlayerDeathArgs>{}.field<NumberFieldType>(
        "playerNumber", &PlayerDeathArgs::playerNumber);

const Schema<EggLaidArgs> EggLaidSchema =
    Schema<EggLaidArgs>{}
        .field<NumberFieldType>("eggNumber", &EggLaidArgs::eggNumber)
        .field<NumberFieldType>("playerNumber", &EggLaidArgs::playerNumber)
        .field<NumberFieldType>("positionX", &EggLaidArgs::positionX)
        .field<NumberFieldType>("positionY", &EggLaidArgs::positionY);

const Schema<EggConnectionArgs> EggConnectionSchema =
    Schema<EggConnectionArgs>{}.field<NumberFieldType>(
        "eggNumber", &EggConnectionArgs::eggNumber);

const Schema<EggDeathArgs> EggDeathSchema =
    Schema<EggDeathArgs>{}.field<NumberFieldType>("eggNumber",
                                                  &EggDeathArgs::eggNumber);

const Schema<TimeUnitArgs> TimeUnitSchema =
    Schema<TimeUnitArgs>{}.field<NumberFieldType>("timeUnit",
                                                  &TimeUnitArgs::timeUnit);

const Schema<EndGameArgs> EndGameSchema =
    Schema<EndGameArgs>{}.field<StringFieldType>("teamName",
                                                 &EndGameArgs::teamName);

const Schema<ServerMessageArgs> ServerMessageSchema =
    Schema<ServerMessageArgs>{}.field<LongStringFieldType>(
        "message", &ServerMessageArgs::message);

const Schema<TileRequestArgs> TileRequestSchema =
    Schema<TileRequestArgs>{}
        .field<NumberFieldType>("positionX", &TileRequestArgs::positionX)
        .field<NumberFieldType>("positionY", &TileRequestArgs::positionY);

const Schema<PlayerNumberArgs> PlayerNumberSchema =
    Schema<PlayerNumberArgs>{}.field<NumberFieldType>(
        "playerNumber", &PlayerNumberArgs::playerNumber);

const zappy::rpc::TypedMessage<MapSizeArgs> MapSize =
    RPCMessage{"msz"}.schema(MapSizeSchema);
const zappy::rpc::TypedMessage<TileContentArgs> TileContent =
    RPCMessage{"bct"}.schema(TileContentSchema);
const zappy::rpc::TypedMessage<TeamNameArgs> TeamName =
    RPCMessage{"tna"}.schema(TeamNameSchema);
const zappy::rpc::TypedMessage<NewPlayerArgs> NewPlayer =
    RPCMessage{"pnw"}.schema(NewPlayerSchema);
const zappy::rpc::TypedMessage<PlayerPositionArgs> PlayerPosition =
    RPCMessage{"ppo"}.schema(PlayerPositionSchema);
const zappy::rpc::TypedMessage<PlayerLevelArgs> PlayerLevel =
    RPCMessage{"plv"}.schema(PlayerLevelSchema);
const zappy::rpc::TypedMessage<PlayerInventoryArgs> PlayerInventory =
    RPCMessage{"pin"}.schema(PlayerInventorySchema);
const zappy::rpc::TypedMessage<ExpulsionArgs> Expulsion =
    RPCMessage{"pex"}.schema(ExpulsionSchema);
const zappy::rpc::TypedMessage<BroadcastArgs> Broadcast =
    RPCMessage{"pbc"}.schema(BroadcastSchema);
const zappy::rpc::TypedMessage<IncantationEndArgs> IncantationEnd =
    RPCMessage{"pie"}.schema(IncantationEndSchema);
const zappy::rpc::TypedMessage<EggLayingArgs> EggLaying =
    RPCMessage{"pfk"}.schema(EggLayingSchema);
const zappy::rpc::TypedMessage<ResourceDropArgs> ResourceDrop =
    RPCMessage{"pdr"}.schema(ResourceDropSchema);
const zappy::rpc::TypedMessage<ResourceCollectArgs> ResourceCollect =
    RPCMessage{"pgt"}.schema(ResourceCollectSchema);
const zappy::rpc::TypedMessage<PlayerDeathArgs> PlayerDeath =
    RPCMessage{"pdi"}.schema(PlayerDeathSchema);
const zappy::rpc::TypedMessage<EggLaidArgs> EggLaid =
    RPCMessage{"enw"}.schema(EggLaidSchema);
const zappy::rpc::TypedMessage<EggConnectionArgs> EggConnection =
    RPCMessage{"ebo"}.schema(EggConnectionSchema);
const zappy::rpc::TypedMessage<EggDeathArgs> EggDeath =
    RPCMessage{"edi"}.schema(EggDeathSchema);
const zappy::rpc::TypedMessage<TimeUnitArgs> TimeUnit =
    RPCMessage{"sgt"}.schema(TimeUnitSchema);
const zappy::rpc::TypedMessage<EndGameArgs> EndGame =
    RPCMessage{"seg"}.schema(EndGameSchema);
const zappy::rpc::TypedMessage<ServerMessageArgs> ServerMessage =
    RPCMessage{"smg"}.schema(ServerMessageSchema);

const zappy::rpc::TypedMessage<TileRequestArgs> RequestTileContent =
    RPCMessage{"bct"}.schema(TileRequestSchema);
const zappy::rpc::TypedMessage<PlayerNumberArgs> RequestPlayerPosition =
    RPCMessage{"ppo"}.schema(PlayerNumberSchema);
const zappy::rpc::TypedMessage<PlayerNumberArgs> RequestPlayerLevel =
    RPCMessage{"plv"}.schema(PlayerNumberSchema);
const zappy::rpc::TypedMessage<PlayerNumberArgs> RequestPlayerInventory =
    RPCMessage{"pin"}.schema(PlayerNumberSchema);
const zappy::rpc::TypedMessage<TimeUnitArgs> SetTimeUnit =
    RPCMessage{"sst"}.schema(TimeUnitSchema);

const RPCMessage RequestMapSize{"msz"};
const RPCMessage RequestMapContent{"mct"};
const RPCMessage RequestTeamNames{"tna"};
const RPCMessage RequestTimeUnit{"sgt"};
const RPCMessage StartIncantation{"pic"};
const RPCMessage UnknownCommand{"suc"};
const RPCMessage BadParameter{"sbp"};

}  // namespace zappy::protocol
