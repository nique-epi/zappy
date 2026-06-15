/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** GuiQueryHandlers — handlers replying to GUI map and team queries
*/

#pragma once

#include "App/ServerConfig.hpp"
#include "App/TimeUnit.hpp"
#include "App/World/Map/Map.hpp"
#include "App/World/Player/PlayerRegistry.hpp"
#include "Net/ClientContext.hpp"
#include "Rpc/Server/RPCServer.hpp"

namespace zappy::server {

/**
 * @brief Wire every GUI query handler on @p server.
 *
 * Registers the GUI request opcodes so a connected GRAPHIC client gets the
 * proper response:
 *
 * - `msz` -> `msz <width> <height>`
 * - `bct X Y` -> `bct X Y q0 q1 q2 q3 q4 q5 q6` (food + six minerals)
 * - `mct` -> one `bct` line per cell of the map (W * H lines)
 * - `tna` -> one `tna <name>` line per team configured at startup
 * - `ppo #n` -> `ppo #n X Y O` (orientation: 1=N, 2=E, 3=S, 4=W)
 * - `plv #n` -> `plv #n L`
 * - `pin #n` -> `pin #n X Y q0 q1 q2 q3 q4 q5 q6`
 * - `sgt` -> `sgt T` (current reciprocal time unit `f`)
 * - `sst T` -> `sst T`, and the new `f` applies to subsequent AI actions
 *
 * Validation failures (non-numeric `bct X Y`, unknown `#n`, non-positive `sst`)
 * reply `sbp`. Unknown GUI opcodes flow through `onUnknown` and reply `suc`.
 *
 * Out-of-range coordinates for `bct` are accepted: the toroidal map wraps any
 * integer coordinate to a valid tile.
 *
 * @param[in]     server   The RPC server hosting both GUI and AI clients.
 * @param[in]     config   Startup configuration (map dimensions, team names).
 *                         Must outlive @p server.
 * @param[in]     map      The live world map; must outlive @p server.
 * @param[in]     players  The live player registry; must outlive @p server.
 * @param[in,out] timeUnit The live reciprocal time unit `f`, read by `sgt` and
 *                         mutated by `sst`; must outlive @p server.
 */
void installGuiQueryHandlers(zappy::rpc::RPCServer<ClientContext>& server,
                             const ServerConfig& config, const world::Map& map,
                             const world::PlayerRegistry& players,
                             TimeUnit& timeUnit);

}  // namespace zappy::server
