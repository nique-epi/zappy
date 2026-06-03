/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** GuiProtocol — message structs, schemas and definitions for the GUI protocol
*/

#pragma once

#include <string>
#include "Rpc/Message/RPCMessage.hpp"
#include "Rpc/Message/TypedMessage.hpp"
#include "Schema/Schema.hpp"

namespace zappy::protocol {

/**
 * @brief Map dimensions (msz X Y).
 */
struct MapSizeArgs {
  std::string width;
  std::string height;
};

/**
 * @brief Tile content with the seven resource quantities (bct X Y q0..q6).
 */
struct TileContentArgs {
  std::string positionX;
  std::string positionY;
  std::string food;
  std::string linemate;
  std::string deraumere;
  std::string sibur;
  std::string mendiane;
  std::string phiras;
  std::string thystame;
};

/**
 * @brief Single team name (tna N).
 */
struct TeamNameArgs {
  std::string teamName;
};

/**
 * @brief New player connection (pnw #n X Y O L N).
 */
struct NewPlayerArgs {
  std::string playerNumber;
  std::string positionX;
  std::string positionY;
  std::string orientation;
  std::string level;
  std::string teamName;
};

/**
 * @brief Player position (ppo #n X Y O).
 */
struct PlayerPositionArgs {
  std::string playerNumber;
  std::string positionX;
  std::string positionY;
  std::string orientation;
};

/**
 * @brief Player level (plv #n L).
 */
struct PlayerLevelArgs {
  std::string playerNumber;
  std::string level;
};

/**
 * @brief Player inventory (pin #n X Y q0..q6).
 */
struct PlayerInventoryArgs {
  std::string playerNumber;
  std::string positionX;
  std::string positionY;
  std::string food;
  std::string linemate;
  std::string deraumere;
  std::string sibur;
  std::string mendiane;
  std::string phiras;
  std::string thystame;
};

/**
 * @brief Player expulsion (pex #n).
 */
struct ExpulsionArgs {
  std::string playerNumber;
};

/**
 * @brief Player broadcast (pbc #n M).
 */
struct BroadcastArgs {
  std::string playerNumber;
  std::string message;
};

/**
 * @brief Incantation end (pie X Y R).
 */
struct IncantationEndArgs {
  std::string positionX;
  std::string positionY;
  std::string result;
};

/**
 * @brief Egg laying by a player (pfk #n).
 */
struct EggLayingArgs {
  std::string playerNumber;
};

/**
 * @brief Resource dropping (pdr #n i).
 */
struct ResourceDropArgs {
  std::string playerNumber;
  std::string resourceNumber;
};

/**
 * @brief Resource collecting (pgt #n i).
 */
struct ResourceCollectArgs {
  std::string playerNumber;
  std::string resourceNumber;
};

/**
 * @brief Player death (pdi #n).
 */
struct PlayerDeathArgs {
  std::string playerNumber;
};

/**
 * @brief Egg laid by a player (enw #e #n X Y).
 */
struct EggLaidArgs {
  std::string eggNumber;
  std::string playerNumber;
  std::string positionX;
  std::string positionY;
};

/**
 * @brief Player connection for an egg (ebo #e).
 */
struct EggConnectionArgs {
  std::string eggNumber;
};

/**
 * @brief Egg death (edi #e).
 */
struct EggDeathArgs {
  std::string eggNumber;
};

/**
 * @brief Time unit value (sgt T as a server reply, sst T both directions).
 */
struct TimeUnitArgs {
  std::string timeUnit;
};

/**
 * @brief End of game (seg N).
 */
struct EndGameArgs {
  std::string teamName;
};

/**
 * @brief Free-form server message (smg M).
 */
struct ServerMessageArgs {
  std::string message;
};

/**
 * @brief Tile content request (bct X Y, GUI to server).
 */
struct TileRequestArgs {
  std::string positionX;
  std::string positionY;
};

/**
 * @brief Single-player request (ppo/plv/pin #n, GUI to server).
 */
struct PlayerNumberArgs {
  std::string playerNumber;
};

extern const zappy::schema::Schema<MapSizeArgs> MapSizeSchema;
extern const zappy::schema::Schema<TileContentArgs> TileContentSchema;
extern const zappy::schema::Schema<TeamNameArgs> TeamNameSchema;
extern const zappy::schema::Schema<NewPlayerArgs> NewPlayerSchema;
extern const zappy::schema::Schema<PlayerPositionArgs> PlayerPositionSchema;
extern const zappy::schema::Schema<PlayerLevelArgs> PlayerLevelSchema;
extern const zappy::schema::Schema<PlayerInventoryArgs> PlayerInventorySchema;
extern const zappy::schema::Schema<ExpulsionArgs> ExpulsionSchema;
extern const zappy::schema::Schema<BroadcastArgs> BroadcastSchema;
extern const zappy::schema::Schema<IncantationEndArgs> IncantationEndSchema;
extern const zappy::schema::Schema<EggLayingArgs> EggLayingSchema;
extern const zappy::schema::Schema<ResourceDropArgs> ResourceDropSchema;
extern const zappy::schema::Schema<ResourceCollectArgs> ResourceCollectSchema;
extern const zappy::schema::Schema<PlayerDeathArgs> PlayerDeathSchema;
extern const zappy::schema::Schema<EggLaidArgs> EggLaidSchema;
extern const zappy::schema::Schema<EggConnectionArgs> EggConnectionSchema;
extern const zappy::schema::Schema<EggDeathArgs> EggDeathSchema;
extern const zappy::schema::Schema<TimeUnitArgs> TimeUnitSchema;
extern const zappy::schema::Schema<EndGameArgs> EndGameSchema;
extern const zappy::schema::Schema<ServerMessageArgs> ServerMessageSchema;
extern const zappy::schema::Schema<TileRequestArgs> TileRequestSchema;
extern const zappy::schema::Schema<PlayerNumberArgs> PlayerNumberSchema;

extern const zappy::rpc::TypedMessage<MapSizeArgs> MapSize;
extern const zappy::rpc::TypedMessage<TileContentArgs> TileContent;
extern const zappy::rpc::TypedMessage<TeamNameArgs> TeamName;
extern const zappy::rpc::TypedMessage<NewPlayerArgs> NewPlayer;
extern const zappy::rpc::TypedMessage<PlayerPositionArgs> PlayerPosition;
extern const zappy::rpc::TypedMessage<PlayerLevelArgs> PlayerLevel;
extern const zappy::rpc::TypedMessage<PlayerInventoryArgs> PlayerInventory;
extern const zappy::rpc::TypedMessage<ExpulsionArgs> Expulsion;
extern const zappy::rpc::TypedMessage<BroadcastArgs> Broadcast;
extern const zappy::rpc::TypedMessage<IncantationEndArgs> IncantationEnd;
extern const zappy::rpc::TypedMessage<EggLayingArgs> EggLaying;
extern const zappy::rpc::TypedMessage<ResourceDropArgs> ResourceDrop;
extern const zappy::rpc::TypedMessage<ResourceCollectArgs> ResourceCollect;
extern const zappy::rpc::TypedMessage<PlayerDeathArgs> PlayerDeath;
extern const zappy::rpc::TypedMessage<EggLaidArgs> EggLaid;
extern const zappy::rpc::TypedMessage<EggConnectionArgs> EggConnection;
extern const zappy::rpc::TypedMessage<EggDeathArgs> EggDeath;
extern const zappy::rpc::TypedMessage<TimeUnitArgs> TimeUnit;
extern const zappy::rpc::TypedMessage<EndGameArgs> EndGame;
extern const zappy::rpc::TypedMessage<ServerMessageArgs> ServerMessage;

extern const zappy::rpc::TypedMessage<TileRequestArgs> RequestTileContent;
extern const zappy::rpc::TypedMessage<PlayerNumberArgs> RequestPlayerPosition;
extern const zappy::rpc::TypedMessage<PlayerNumberArgs> RequestPlayerLevel;
extern const zappy::rpc::TypedMessage<PlayerNumberArgs> RequestPlayerInventory;
extern const zappy::rpc::TypedMessage<TimeUnitArgs> SetTimeUnit;

extern const zappy::rpc::RPCMessage RequestMapSize;
extern const zappy::rpc::RPCMessage RequestMapContent;
extern const zappy::rpc::RPCMessage RequestTeamNames;
extern const zappy::rpc::RPCMessage RequestTimeUnit;
extern const zappy::rpc::RPCMessage StartIncantation;
extern const zappy::rpc::RPCMessage UnknownCommand;
extern const zappy::rpc::RPCMessage BadParameter;

}  // namespace zappy::protocol
