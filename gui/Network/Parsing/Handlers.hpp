/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Handlers — registration entry points for every server message handler
*/

#pragma once

#include "Network/Parsing/HandlerTypes.hpp"
#include "World/WorldState.hpp"

namespace zappy::gui {

/**
 * @brief Register handlers for map messages into @p handlers.
 *
 * Covers: msz (map size), bct (tile content).
 */
void registerMapHandlers(HandlerMap& handlers, WorldState& world);

/**
 * @brief Register handlers for player messages into @p handlers.
 *
 * Covers: pnw, ppo, plv, pin, pdi, pdr, pgt.
 */
void registerPlayerHandlers(HandlerMap& handlers, WorldState& world);

/**
 * @brief Register handlers for egg messages into @p handlers.
 *
 * Covers: enw (new egg), ebo (egg hatches), edi (egg dies).
 */
void registerEggHandlers(HandlerMap& handlers, WorldState& world);

/**
 * @brief Register handlers for server-level messages into @p handlers.
 *
 * Covers: tna, sgt, sst, seg, suc, sbp and transient events
 * (pex, pbc, pic, pie, pfk, smg).
 */
void registerServerHandlers(HandlerMap& handlers, WorldState& world);

}  // namespace zappy::gui
