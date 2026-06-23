/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** GuiEventBroadcaster — pushes live game events to every connected GUI client
*/

#pragma once

#include <string>
#include "App/World/Resources/ResourceType.hpp"
#include "Net/ClientContext.hpp"
#include "Rpc/Server/RPCServer.hpp"

namespace zappy::world {
class Player;
class Tile;
}  // namespace zappy::world

namespace zappy::server {

/**
 * @brief Fan-out of unsolicited GUI events to every GRAPHIC client.
 *
 * Business code calls one method per state change; the broadcaster formats the
 * matching protocol line and sends it to every session whose @ref
 * ClientType is @c Gui — AI sessions are skipped. The events are dedicated and
 * minimal: each call pushes exactly what changed (a player, a tile) instead of
 * a global map dump, so a busy server never floods the GUI with redundant
 * lines.
 *
 * The referenced @p server is owned by the @ref GameServer and outlives the
 * broadcaster.
 */
class GuiEventBroadcaster {
 public:
  explicit GuiEventBroadcaster(zappy::rpc::RPCServer<ClientContext>& server);

  /// New player connection: `pnw #n X Y O L N`.
  void playerConnected(const world::Player& player);

  /// Player position or orientation change: `ppo #n X Y O`.
  void playerMoved(const world::Player& player);

  /// Player elevation: `plv #n L`.
  void playerLevel(const world::Player& player);

  /// Player inventory snapshot: `pin #n X Y q0..q6`.
  void playerInventory(const world::Player& player);

  /// Player death: `pdi #n`.
  void playerDied(int playerId);

  /// Resource picked up from the floor: `pgt #n i`.
  void resourceCollected(int playerId, world::ResourceType resource);

  /// Resource dropped onto the floor: `pdr #n i`.
  void resourceDropped(int playerId, world::ResourceType resource);

  /// Tile content change (e.g. periodic respawn): `bct X Y q0..q6`.
  void tileChanged(int column, int row, const world::Tile& tile);

 private:
  void broadcast(const std::string& line);

  zappy::rpc::RPCServer<ClientContext>& server_;
};

}  // namespace zappy::server
