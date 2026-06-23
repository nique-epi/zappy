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

float tileToWorld(int gridCoord);
int worldToTile(float worldCoord);
Color teamColor(const WorldState& world, const std::string& teamName);

}  // namespace zappy::gui
