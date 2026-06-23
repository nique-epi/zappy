/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** WorldQuery — read-only lookups over WorldState (no rendering dependency)
*/

#pragma once

#include "World/WorldState.hpp"

namespace zappy::gui {

/**
 * @brief Find the player with @p playerId, or nullptr if absent.
 *
 * Lives outside RenderUtils so consumers that only query the world (e.g.
 * @ref PlayerSelection) stay free of any raylib dependency and unit-testable.
 */
const Player* findPlayerById(const WorldState& world, int playerId);

}  // namespace zappy::gui
