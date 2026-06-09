/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** MapHandlers — handlers for map-related server messages (msz, bct)
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

}  // namespace zappy::gui
