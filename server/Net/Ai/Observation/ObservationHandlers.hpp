/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ObservationHandlers — AI Look/Inventory/Connect_nbr command handlers
*/

#pragma once

#include "Net/Ai/AiHandlerContext.hpp"
#include "Net/ClientContext.hpp"
#include "Rpc/Server/RPCServer.hpp"

namespace zappy::server {

/**
 * @brief Register the drone observation commands on @p server.
 *
 * Binds `Look` (visible tiles in subject numbering), `Inventory` (the seven
 * resources with their quantities) and `Connect_nbr` (the team's free-slot
 * count). `Look` and `Inventory` reply `ko` when the session owns no drone.
 *
 * @param[in,out] server  RPC server the handlers are registered on.
 * @param[in]     context World dependencies the handlers operate on.
 */
void installObservationHandlers(zappy::rpc::RPCServer<ClientContext>& server,
                                const AiHandlerContext& context);

}  // namespace zappy::server
