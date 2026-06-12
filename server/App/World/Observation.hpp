/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Observation — textual views a drone reports: its surroundings and its stock
*/

#pragma once

#include <string>
#include "App/World/Player/Direction.hpp"

namespace zappy::world {

class Map;
class Inventory;

/**
 * @brief Build the AI `Look` response for a drone standing on
 *        (@p column, @p row) facing @p facing with vision range @p level.
 *
 * Tiles are listed in the subject's numbering: index 0 is the drone's own
 * tile, then each forward row from the closest to the farthest, and within a
 * row from the drone's left to its right. Row @e d (0..@p level) holds
 * @e 2d+1 tiles, so the response carries @e (level+1)^2 tiles. Each tile lists
 * its objects separated by spaces — one `player` token per drone present, then
 * every resource unit by name — and tiles are separated by commas, wrapped in
 * brackets (e.g. `[player,,linemate food]`). Coordinates wrap toroidally on
 * @p map. The string carries no trailing newline.
 *
 * @param[in] column Drone column on the X axis.
 * @param[in] row    Drone row on the Y axis.
 * @param[in] facing Drone orientation.
 * @param[in] level  Drone elevation level; the forward vision range in rows.
 * @param[in] map    World queried for tile contents.
 * @returns The bracketed, comma-separated Look response.
 */
[[nodiscard]] std::string formatLook(int column, int row, Direction facing,
                                     int level, const Map& map);

/**
 * @brief Build the AI `Inventory` response for @p inventory.
 *
 * Lists the seven resource kinds in protocol order (food first, then the six
 * minerals) as `name quantity` pairs separated by `, ` and wrapped in brackets
 * (e.g. `[food 10, linemate 0, ...]`). The food count doubles as the drone's
 * remaining life span. The string carries no trailing newline.
 *
 * @param[in] inventory Drone stock to serialise.
 * @returns The bracketed, comma-separated Inventory response.
 */
[[nodiscard]] std::string formatInventory(const Inventory& inventory);

}  // namespace zappy::world
