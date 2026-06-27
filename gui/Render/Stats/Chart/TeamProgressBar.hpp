/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** TeamProgressBar — level-distribution histogram for one team
*/

#pragma once

#include <raylib.h>
#include <string>

namespace zappy::gui {

struct WorldState;

/**
 * @brief Draw a level-distribution histogram for a single team.
 *
 * Eight columns represent levels 1–8. Each bar is filled proportionally to
 * the number of alive players at that level relative to the most-populated
 * level. Level labels are drawn below the chart.
 *
 * @param[in] origin     Top-left corner of the chart area.
 * @param[in] width      Total width available for the chart.
 * @param[in] world      Live world state (player list queried every frame).
 * @param[in] teamName   Team whose player-level distribution is charted.
 * @param[in] barColor   Accent color used to fill the bars.
 */
void drawTeamProgressBar(Vector2 origin, float width, const WorldState& world,
                         const std::string& teamName, Color barColor);

}  // namespace zappy::gui
