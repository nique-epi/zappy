/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** AiHandlerSupport — helpers shared by the AI command handlers
*/

#pragma once

#include "Net/Ai/AiHandlerContext.hpp"
#include "Net/Ai/AiTypes.hpp"

namespace zappy::world {
class Player;
}  // namespace zappy::world

namespace zappy::server {

/**
 * @brief Resolve the drone bound to @p session, or reply `ko` when none is.
 *
 * Centralises the guard shared by every drone-scoped AI handler: it looks the
 * player up in the registry and, when the session owns no drone, sends the
 * `ko` reply before returning nullptr so the caller can simply early-return.
 *
 * @param[in,out] session Session whose bound drone is resolved.
 * @param[in]     context World dependencies the lookup uses.
 * @returns The bound drone, or nullptr after a `ko` reply was sent.
 */
[[nodiscard]] world::Player* findPlayerOrReplyKo(
    AiSession& session, const AiHandlerContext& context);

}  // namespace zappy::server
