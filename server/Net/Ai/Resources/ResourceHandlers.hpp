/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ResourceHandlers — AI Take/Set command handlers
*/

#pragma once

#include "Net/Ai/AiHandlerContext.hpp"
#include "Net/ClientContext.hpp"
#include "Rpc/Server/RPCServer.hpp"

namespace zappy::server {

/**
 * @brief Register the resource manipulation commands on @p server.
 *
 * Binds `Take` and `Set`, which move one resource unit between the drone's
 * tile and its inventory. Each replies `ok` on success, or `ko` when the
 * session owns no drone, the object name is unknown, or the resource is
 * unavailable.
 *
 * @param[in,out] server  RPC server the handlers are registered on.
 * @param[in]     context World dependencies the handlers operate on.
 */
void installResourceHandlers(zappy::rpc::RPCServer<ClientContext>& server,
                             const AiHandlerContext& context);

}  // namespace zappy::server
