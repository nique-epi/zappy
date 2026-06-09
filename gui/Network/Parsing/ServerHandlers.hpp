/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ServerHandlers — handlers for server-level messages
*/

#pragma once

#include "Network/Parsing/HandlerTypes.hpp"
#include "World/WorldState.hpp"

namespace zappy::gui {

/**
 * @brief Register handlers for server-level messages into @p handlers.
 *
 * Covers: tna, sgt, sst, seg, suc, sbp and transient events
 * (pex, pbc, pic, pie, pfk, smg).
 */
void registerServerHandlers(HandlerMap& handlers, WorldState& world);

}  // namespace zappy::gui
