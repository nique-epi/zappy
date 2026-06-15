/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** GuiProtocol — message structs, schemas and definitions for the GUI protocol
*/

#include "Protocol/GuiProtocol.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include "Schema/Fields/Fields.hpp"

namespace zappy::protocol {

using zappy::rpc::RPCMessage;
using zappy::rpc::TypedMessage;
using zappy::schema::LongStringFieldType;
using zappy::schema::NumberFieldType;
using zappy::schema::Schema;
using zappy::schema::StringFieldType;

namespace {

std::shared_ptr<const Schema<MapSizeArgs>> mapSizeSchema() {
  static const auto schema = std::make_shared<const Schema<MapSizeArgs>>(
      Schema<MapSizeArgs>{}
          .field<NumberFieldType>("width", &MapSizeArgs::width)
          .field<NumberFieldType>("height", &MapSizeArgs::height));
  return schema;
}

std::shared_ptr<const Schema<TileContentArgs>> tileContentSchema() {
  static const auto schema = std::make_shared<const Schema<TileContentArgs>>(
      Schema<TileContentArgs>{}
          .field<NumberFieldType>("positionX", &TileContentArgs::positionX)
          .field<NumberFieldType>("positionY", &TileContentArgs::positionY)
          .field<NumberFieldType>("food", &TileContentArgs::food)
          .field<NumberFieldType>("linemate", &TileContentArgs::linemate)
          .field<NumberFieldType>("deraumere", &TileContentArgs::deraumere)
          .field<NumberFieldType>("sibur", &TileContentArgs::sibur)
          .field<NumberFieldType>("mendiane", &TileContentArgs::mendiane)
          .field<NumberFieldType>("phiras", &TileContentArgs::phiras)
          .field<NumberFieldType>("thystame", &TileContentArgs::thystame));
  return schema;
}

std::shared_ptr<const Schema<TeamNameArgs>> teamNameSchema() {
  static const auto schema = std::make_shared<const Schema<TeamNameArgs>>(
      Schema<TeamNameArgs>{}.field<StringFieldType>("teamName",
                                                    &TeamNameArgs::teamName));
  return schema;
}

std::shared_ptr<const Schema<NewPlayerArgs>> newPlayerSchema() {
  static const auto schema = std::make_shared<const Schema<NewPlayerArgs>>(
      Schema<NewPlayerArgs>{}
          .field<NumberFieldType>("playerNumber", &NewPlayerArgs::playerNumber)
          .field<NumberFieldType>("positionX", &NewPlayerArgs::positionX)
          .field<NumberFieldType>("positionY", &NewPlayerArgs::positionY)
          .field<NumberFieldType>("orientation", &NewPlayerArgs::orientation)
          .field<NumberFieldType>("level", &NewPlayerArgs::level)
          .field<StringFieldType>("teamName", &NewPlayerArgs::teamName));
  return schema;
}

std::shared_ptr<const Schema<PlayerPositionArgs>> playerPositionSchema() {
  static const auto schema = std::make_shared<const Schema<PlayerPositionArgs>>(
      Schema<PlayerPositionArgs>{}
          .field<NumberFieldType>("playerNumber",
                                  &PlayerPositionArgs::playerNumber)
          .field<NumberFieldType>("positionX", &PlayerPositionArgs::positionX)
          .field<NumberFieldType>("positionY", &PlayerPositionArgs::positionY)
          .field<NumberFieldType>("orientation",
                                  &PlayerPositionArgs::orientation));
  return schema;
}

std::shared_ptr<const Schema<PlayerLevelArgs>> playerLevelSchema() {
  static const auto schema = std::make_shared<const Schema<PlayerLevelArgs>>(
      Schema<PlayerLevelArgs>{}
          .field<NumberFieldType>("playerNumber",
                                  &PlayerLevelArgs::playerNumber)
          .field<NumberFieldType>("level", &PlayerLevelArgs::level));
  return schema;
}

std::shared_ptr<const Schema<PlayerInventoryArgs>> playerInventorySchema() {
  static const auto schema = std::make_shared<
      const Schema<PlayerInventoryArgs>>(
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
          .field<NumberFieldType>("thystame", &PlayerInventoryArgs::thystame));
  return schema;
}

std::shared_ptr<const Schema<ExpulsionArgs>> expulsionSchema() {
  static const auto schema = std::make_shared<const Schema<ExpulsionArgs>>(
      Schema<ExpulsionArgs>{}.field<NumberFieldType>(
          "playerNumber", &ExpulsionArgs::playerNumber));
  return schema;
}

std::shared_ptr<const Schema<BroadcastArgs>> broadcastSchema() {
  static const auto schema = std::make_shared<const Schema<BroadcastArgs>>(
      Schema<BroadcastArgs>{}
          .field<NumberFieldType>("playerNumber", &BroadcastArgs::playerNumber)
          .field<LongStringFieldType>("message", &BroadcastArgs::message));
  return schema;
}

std::shared_ptr<const Schema<IncantationEndArgs>> incantationEndSchema() {
  static const auto schema = std::make_shared<const Schema<IncantationEndArgs>>(
      Schema<IncantationEndArgs>{}
          .field<NumberFieldType>("positionX", &IncantationEndArgs::positionX)
          .field<NumberFieldType>("positionY", &IncantationEndArgs::positionY)
          .field<NumberFieldType>("result", &IncantationEndArgs::result));
  return schema;
}

std::shared_ptr<const Schema<EggLayingArgs>> eggLayingSchema() {
  static const auto schema = std::make_shared<const Schema<EggLayingArgs>>(
      Schema<EggLayingArgs>{}.field<NumberFieldType>(
          "playerNumber", &EggLayingArgs::playerNumber));
  return schema;
}

std::shared_ptr<const Schema<ResourceDropArgs>> resourceDropSchema() {
  static const auto schema = std::make_shared<const Schema<ResourceDropArgs>>(
      Schema<ResourceDropArgs>{}
          .field<NumberFieldType>("playerNumber",
                                  &ResourceDropArgs::playerNumber)
          .field<NumberFieldType>("resourceNumber",
                                  &ResourceDropArgs::resourceNumber));
  return schema;
}

std::shared_ptr<const Schema<ResourceCollectArgs>> resourceCollectSchema() {
  static const auto schema =
      std::make_shared<const Schema<ResourceCollectArgs>>(
          Schema<ResourceCollectArgs>{}
              .field<NumberFieldType>("playerNumber",
                                      &ResourceCollectArgs::playerNumber)
              .field<NumberFieldType>("resourceNumber",
                                      &ResourceCollectArgs::resourceNumber));
  return schema;
}

std::shared_ptr<const Schema<PlayerDeathArgs>> playerDeathSchema() {
  static const auto schema = std::make_shared<const Schema<PlayerDeathArgs>>(
      Schema<PlayerDeathArgs>{}.field<NumberFieldType>(
          "playerNumber", &PlayerDeathArgs::playerNumber));
  return schema;
}

std::shared_ptr<const Schema<EggLaidArgs>> eggLaidSchema() {
  static const auto schema = std::make_shared<const Schema<EggLaidArgs>>(
      Schema<EggLaidArgs>{}
          .field<NumberFieldType>("eggNumber", &EggLaidArgs::eggNumber)
          .field<NumberFieldType>("playerNumber", &EggLaidArgs::playerNumber)
          .field<NumberFieldType>("positionX", &EggLaidArgs::positionX)
          .field<NumberFieldType>("positionY", &EggLaidArgs::positionY));
  return schema;
}

std::shared_ptr<const Schema<EggConnectionArgs>> eggConnectionSchema() {
  static const auto schema = std::make_shared<const Schema<EggConnectionArgs>>(
      Schema<EggConnectionArgs>{}.field<NumberFieldType>(
          "eggNumber", &EggConnectionArgs::eggNumber));
  return schema;
}

std::shared_ptr<const Schema<EggDeathArgs>> eggDeathSchema() {
  static const auto schema = std::make_shared<const Schema<EggDeathArgs>>(
      Schema<EggDeathArgs>{}.field<NumberFieldType>("eggNumber",
                                                    &EggDeathArgs::eggNumber));
  return schema;
}

std::shared_ptr<const Schema<TimeUnitArgs>> timeUnitSchema() {
  static const auto schema = std::make_shared<const Schema<TimeUnitArgs>>(
      Schema<TimeUnitArgs>{}.field<NumberFieldType>("timeUnit",
                                                    &TimeUnitArgs::timeUnit));
  return schema;
}

std::shared_ptr<const Schema<EndGameArgs>> endGameSchema() {
  static const auto schema = std::make_shared<const Schema<EndGameArgs>>(
      Schema<EndGameArgs>{}.field<StringFieldType>("teamName",
                                                   &EndGameArgs::teamName));
  return schema;
}

std::shared_ptr<const Schema<ServerMessageArgs>> serverMessageSchema() {
  static const auto schema = std::make_shared<const Schema<ServerMessageArgs>>(
      Schema<ServerMessageArgs>{}.field<LongStringFieldType>(
          "message", &ServerMessageArgs::message));
  return schema;
}

std::shared_ptr<const Schema<TileRequestArgs>> tileRequestSchema() {
  static const auto schema = std::make_shared<const Schema<TileRequestArgs>>(
      Schema<TileRequestArgs>{}
          .field<NumberFieldType>("positionX", &TileRequestArgs::positionX)
          .field<NumberFieldType>("positionY", &TileRequestArgs::positionY));
  return schema;
}

std::shared_ptr<const Schema<PlayerNumberArgs>> playerNumberSchema() {
  static const auto schema = std::make_shared<const Schema<PlayerNumberArgs>>(
      Schema<PlayerNumberArgs>{}.field<StringFieldType>(
          "playerNumber", &PlayerNumberArgs::playerNumber));
  return schema;
}

const RPCMessage& rawMessage(const char* opcode) {
  static std::unordered_map<std::string, RPCMessage> cache;
  auto [iterator, inserted] = cache.try_emplace(opcode, RPCMessage{opcode});
  return iterator->second;
}

}  // namespace

const TypedMessage<MapSizeArgs>& MapSize() {
  static const auto message = RPCMessage{"msz"}.schema(mapSizeSchema());
  return message;
}

const TypedMessage<TileContentArgs>& TileContent() {
  static const auto message = RPCMessage{"bct"}.schema(tileContentSchema());
  return message;
}

const TypedMessage<TeamNameArgs>& TeamName() {
  static const auto message = RPCMessage{"tna"}.schema(teamNameSchema());
  return message;
}

const TypedMessage<NewPlayerArgs>& NewPlayer() {
  static const auto message = RPCMessage{"pnw"}.schema(newPlayerSchema());
  return message;
}

const TypedMessage<PlayerPositionArgs>& PlayerPosition() {
  static const auto message = RPCMessage{"ppo"}.schema(playerPositionSchema());
  return message;
}

const TypedMessage<PlayerLevelArgs>& PlayerLevel() {
  static const auto message = RPCMessage{"plv"}.schema(playerLevelSchema());
  return message;
}

const TypedMessage<PlayerInventoryArgs>& PlayerInventory() {
  static const auto message = RPCMessage{"pin"}.schema(playerInventorySchema());
  return message;
}

const TypedMessage<ExpulsionArgs>& Expulsion() {
  static const auto message = RPCMessage{"pex"}.schema(expulsionSchema());
  return message;
}

const TypedMessage<BroadcastArgs>& Broadcast() {
  static const auto message = RPCMessage{"pbc"}.schema(broadcastSchema());
  return message;
}

const TypedMessage<IncantationEndArgs>& IncantationEnd() {
  static const auto message = RPCMessage{"pie"}.schema(incantationEndSchema());
  return message;
}

const TypedMessage<EggLayingArgs>& EggLaying() {
  static const auto message = RPCMessage{"pfk"}.schema(eggLayingSchema());
  return message;
}

const TypedMessage<ResourceDropArgs>& ResourceDrop() {
  static const auto message = RPCMessage{"pdr"}.schema(resourceDropSchema());
  return message;
}

const TypedMessage<ResourceCollectArgs>& ResourceCollect() {
  static const auto message = RPCMessage{"pgt"}.schema(resourceCollectSchema());
  return message;
}

const TypedMessage<PlayerDeathArgs>& PlayerDeath() {
  static const auto message = RPCMessage{"pdi"}.schema(playerDeathSchema());
  return message;
}

const TypedMessage<EggLaidArgs>& EggLaid() {
  static const auto message = RPCMessage{"enw"}.schema(eggLaidSchema());
  return message;
}

const TypedMessage<EggConnectionArgs>& EggConnection() {
  static const auto message = RPCMessage{"ebo"}.schema(eggConnectionSchema());
  return message;
}

const TypedMessage<EggDeathArgs>& EggDeath() {
  static const auto message = RPCMessage{"edi"}.schema(eggDeathSchema());
  return message;
}

const TypedMessage<TimeUnitArgs>& TimeUnit() {
  static const auto message = RPCMessage{"sgt"}.schema(timeUnitSchema());
  return message;
}

const TypedMessage<EndGameArgs>& EndGame() {
  static const auto message = RPCMessage{"seg"}.schema(endGameSchema());
  return message;
}

const TypedMessage<ServerMessageArgs>& ServerMessage() {
  static const auto message = RPCMessage{"smg"}.schema(serverMessageSchema());
  return message;
}

const TypedMessage<TileRequestArgs>& RequestTileContent() {
  static const auto message = RPCMessage{"bct"}.schema(tileRequestSchema());
  return message;
}

const TypedMessage<PlayerNumberArgs>& RequestPlayerPosition() {
  static const auto message = RPCMessage{"ppo"}.schema(playerNumberSchema());
  return message;
}

const TypedMessage<PlayerNumberArgs>& RequestPlayerLevel() {
  static const auto message = RPCMessage{"plv"}.schema(playerNumberSchema());
  return message;
}

const TypedMessage<PlayerNumberArgs>& RequestPlayerInventory() {
  static const auto message = RPCMessage{"pin"}.schema(playerNumberSchema());
  return message;
}

const TypedMessage<TimeUnitArgs>& SetTimeUnit() {
  static const auto message = RPCMessage{"sst"}.schema(timeUnitSchema());
  return message;
}

const RPCMessage& RequestMapSize() { return rawMessage("msz"); }
const RPCMessage& RequestMapContent() { return rawMessage("mct"); }
const RPCMessage& RequestTeamNames() { return rawMessage("tna"); }
const RPCMessage& RequestTimeUnit() { return rawMessage("sgt"); }
const RPCMessage& StartIncantation() { return rawMessage("pic"); }
const RPCMessage& UnknownCommand() { return rawMessage("suc"); }
const RPCMessage& BadParameter() { return rawMessage("sbp"); }

}  // namespace zappy::protocol
