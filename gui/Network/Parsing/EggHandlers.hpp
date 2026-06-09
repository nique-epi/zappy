/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** EggHandlers — handlers for egg-related server messages (enw, ebo, edi)
*/

#pragma once

#include "Network/Parsing/HandlerTypes.hpp"
#include "World/WorldState.hpp"

namespace zappy::gui {

/**
 * @brief Register handlers for egg messages into @p handlers.
 *
 * Covers: enw (new egg), ebo (egg hatches), edi (egg dies).
 */
void registerEggHandlers(HandlerMap& handlers, WorldState& world);

}  // namespace zappy::gui
