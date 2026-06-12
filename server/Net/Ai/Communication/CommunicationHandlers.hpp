/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** CommunicationHandlers — AI Broadcast command handler
*/

#pragma once

#include "Net/Ai/AiHandlerContext.hpp"
#include "Net/ClientContext.hpp"
#include "Rpc/Server/RPCServer.hpp"

namespace zappy::server {

/**
 * @brief Register the sound communication command on @p server.
 *
 * Binds `Broadcast`: it replies `ok` to the emitter and pushes
 * `message K, text` to every AI client, where K is the tile the sound reaches
 * that listener from, computed per receiver along the shortest toroidal path.
 * The emitter stays anonymous. Replies `ko` when the session owns no drone.
 *
 * @param[in,out] server  RPC server the handler is registered on; also used to
 *                        reach every listening drone's session.
 * @param[in]     context World dependencies the handler operates on.
 */
void installCommunicationHandlers(zappy::rpc::RPCServer<ClientContext>& server,
                                  const AiHandlerContext& context);

}  // namespace zappy::server
