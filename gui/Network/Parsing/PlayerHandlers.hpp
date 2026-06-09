/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** PlayerHandlers — handlers for player-related server messages
*/

#pragma once

#include "Network/Parsing/HandlerTypes.hpp"
#include "World/WorldState.hpp"

namespace zappy::gui {

/**
 * @brief Register handlers for player messages into @p handlers.
 *
 * Covers: pnw, ppo, plv, pin, pdi, pdr, pgt.
 */
void registerPlayerHandlers(HandlerMap& handlers, WorldState& world);

}  // namespace zappy::gui
