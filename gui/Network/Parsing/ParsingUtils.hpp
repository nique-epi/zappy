/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ParsingUtils — shared helpers used across all server message handlers
*/

#pragma once

#include <sstream>
#include "World/WorldState.hpp"

namespace zappy::gui {

int parseId(std::istringstream& stream);
Orientation toOrientation(int value);
Player* findPlayer(WorldState& world, int playerId);
Tile* findTile(WorldState& world, int posX, int posY);

}  // namespace zappy::gui
