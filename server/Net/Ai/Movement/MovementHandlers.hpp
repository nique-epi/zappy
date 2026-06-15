/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** MovementHandlers — AI Forward/Right/Left command handlers
*/

#pragma once

#include "Net/Ai/AiHandlerContext.hpp"
#include "Net/Ai/AiTypes.hpp"

namespace zappy::server {

/**
 * @brief Register the drone movement commands on @p server.
 *
 * Binds `Forward` (advance one tile, toroidal), `Right` and `Left` (turn 90
 * degrees). Each replies `ok`, or `ko` when the session owns no drone.
 *
 * @param[in,out] server  RPC server the handlers are registered on.
 * @param[in]     context World dependencies the handlers operate on.
 */
void installMovementHandlers(AiServer& server, const AiHandlerContext& context);

}  // namespace zappy::server
