/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ReproductionHandlers — AI Fork/Eject command handlers
*/

#pragma once

#include "Net/Ai/AiHandlerContext.hpp"
#include "Net/ClientContext.hpp"
#include "Rpc/Server/RPCServer.hpp"

namespace zappy::server {

/**
 * @brief Register the reproduction and conflict commands on @p server.
 *
 * Binds `Fork` (lay an egg, adding a free team slot) and `Eject` (push every
 * other drone off the tile and wipe its eggs). `Fork` replies `ok`, or `ko`
 * when the session owns no drone; `Eject` replies `ok` when at least one drone
 * was pushed, `ko` otherwise, and tells each pushed drone its `eject: K` code.
 *
 * @param[in,out] server  RPC server the handlers are registered on; also used
 *                        to reach the pushed drones' sessions.
 * @param[in]     context World dependencies the handlers operate on.
 */
void installReproductionHandlers(zappy::rpc::RPCServer<ClientContext>& server,
                                 const AiHandlerContext& context);

}  // namespace zappy::server
