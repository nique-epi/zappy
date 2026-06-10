/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** GuiQueryHandlers — handlers replying to GUI map and team queries
*/

#pragma once

#include "App/ServerConfig.hpp"
#include "App/World/Map/Map.hpp"
#include "Net/ClientContext.hpp"
#include "Rpc/Server/RPCServer.hpp"

namespace zappy::server {

/**
 * @brief Wire the GUI query handlers (msz, bct, mct, tna) on @p server.
 *
 * Registers the four GUI request opcodes so a connected GRAPHIC client gets
 * the proper response:
 *
 * - `msz` -> `msz <width> <height>`
 * - `bct X Y` -> `bct X Y q0 q1 q2 q3 q4 q5 q6` (food + six minerals)
 * - `mct` -> one `bct` line per cell of the map (W * H lines)
 * - `tna` -> one `tna <name>` line per team configured at startup
 *
 * Validation failures (non-numeric `bct X Y`, missing fields) flow through
 * the dispatcher's `onInvalid` hook and reply `sbp` to GUI clients. Unknown
 * GUI opcodes flow through `onUnknown` and reply `suc`. Both behaviours are
 * already wired by GameServer's `registerFallbacks`.
 *
 * Out-of-range coordinates for `bct` are accepted: the toroidal map wraps
 * any integer coordinate to a valid tile, so a GUI request for tile (-1, -1)
 * returns the cell at (width - 1, height - 1). This matches the in-engine
 * semantics of @ref world::Map::tileAt.
 *
 * @param[in] server The RPC server hosting both GUI and AI clients.
 * @param[in] config Configuration read at startup; supplies map dimensions
 *                   and team names. Must outlive @p server.
 * @param[in] map    The live world map; must outlive @p server.
 */
void installGuiQueryHandlers(zappy::rpc::RPCServer<ClientContext>& server,
                             const ServerConfig& config, const world::Map& map);

}  // namespace zappy::server
