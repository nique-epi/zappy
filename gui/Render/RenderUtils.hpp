/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** RenderUtils — shared geometry and world-query helpers for the renderers
*/

#pragma once

#include <raylib.h>
#include <string>

namespace zappy::gui {

struct WorldState;
struct Player;

float tileToWorld(int gridCoord);
int worldToTile(float worldCoord);
Color teamColor(const WorldState& world, const std::string& teamName);
const Player* findPlayerById(const WorldState& world, int playerId);

}  // namespace zappy::gui
