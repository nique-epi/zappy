/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ResourceTransfer — move resource units between a tile and a drone inventory
*/

#pragma once

#include "App/World/Resources/ResourceType.hpp"

namespace zappy::world {

class Player;
class Tile;

/**
 * @brief Pick up one unit of @p type from @p tile into @p player's inventory
 *        (the AI `Take` command).
 *
 * @param[in,out] player Drone collecting the resource.
 * @param[in,out] tile   Tile the drone stands on.
 * @param[in]     type   Resource kind to pick up.
 * @returns true if the tile held at least one unit and it was transferred;
 *          false otherwise (nothing changed).
 */
[[nodiscard]] bool takeResource(Player& player, Tile& tile, ResourceType type);

/**
 * @brief Drop one unit of @p type from @p player's inventory onto @p tile
 *        (the AI `Set` command).
 *
 * @param[in,out] player Drone releasing the resource.
 * @param[in,out] tile   Tile the drone stands on.
 * @param[in]     type   Resource kind to drop.
 * @returns true if the drone carried at least one unit and it was transferred;
 *          false otherwise (nothing changed).
 */
[[nodiscard]] bool dropResource(Player& player, Tile& tile, ResourceType type);

}  // namespace zappy::world
