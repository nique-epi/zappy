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

const zappy::rpc::TypedMessage<MapSizeArgs> &MapSize();
const zappy::rpc::TypedMessage<TileContentArgs> &TileContent();
const zappy::rpc::TypedMessage<TeamNameArgs> &TeamName();
const zappy::rpc::TypedMessage<NewPlayerArgs> &NewPlayer();
const zappy::rpc::TypedMessage<PlayerPositionArgs> &PlayerPosition();
const zappy::rpc::TypedMessage<PlayerLevelArgs> &PlayerLevel();
const zappy::rpc::TypedMessage<PlayerInventoryArgs> &PlayerInventory();
const zappy::rpc::TypedMessage<ExpulsionArgs> &Expulsion();
const zappy::rpc::TypedMessage<BroadcastArgs> &Broadcast();
const zappy::rpc::TypedMessage<IncantationEndArgs> &IncantationEnd();
const zappy::rpc::TypedMessage<EggLayingArgs> &EggLaying();
const zappy::rpc::TypedMessage<ResourceDropArgs> &ResourceDrop();
const zappy::rpc::TypedMessage<ResourceCollectArgs> &ResourceCollect();
const zappy::rpc::TypedMessage<PlayerDeathArgs> &PlayerDeath();
const zappy::rpc::TypedMessage<EggLaidArgs> &EggLaid();
const zappy::rpc::TypedMessage<EggConnectionArgs> &EggConnection();
const zappy::rpc::TypedMessage<EggDeathArgs> &EggDeath();
const zappy::rpc::TypedMessage<TimeUnitArgs> &TimeUnit();
const zappy::rpc::TypedMessage<EndGameArgs> &EndGame();
const zappy::rpc::TypedMessage<ServerMessageArgs> &ServerMessage();

const zappy::rpc::TypedMessage<TileRequestArgs> &RequestTileContent();
const zappy::rpc::TypedMessage<PlayerNumberArgs> &RequestPlayerPosition();
const zappy::rpc::TypedMessage<PlayerNumberArgs> &RequestPlayerLevel();
const zappy::rpc::TypedMessage<PlayerNumberArgs> &RequestPlayerInventory();
const zappy::rpc::TypedMessage<TimeUnitArgs> &SetTimeUnit();

const zappy::rpc::RPCMessage &RequestMapSize();
const zappy::rpc::RPCMessage &RequestMapContent();
const zappy::rpc::RPCMessage &RequestTeamNames();
const zappy::rpc::RPCMessage &RequestTimeUnit();
const zappy::rpc::RPCMessage &StartIncantation();
const zappy::rpc::RPCMessage &UnknownCommand();
const zappy::rpc::RPCMessage &BadParameter();

}  // namespace zappy::protocol
