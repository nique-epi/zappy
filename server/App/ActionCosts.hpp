/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ActionCosts — game-time cost of every AI protocol opcode
*/

#pragma once

#include <string_view>

namespace zappy::server {

/// Cost applied to any AI opcode not in the subject table.
constexpr int defaultActionCost = 7;

/**
 * @brief Game time units an AI opcode consumes before the server replies.
 *
 * Values come straight from the Zappy subject: Forward / Right / Left /
 * Take / Set / Look / Eject / Broadcast cost 7, Inventory costs 1, Fork
 * costs 42, Incantation costs 300, Connect_nbr is free. An unknown opcode
 * falls back to @ref defaultActionCost so a malformed line still blocks the
 * player for a finite slice instead of being free.
 *
 * @param[in] opcode AI protocol opcode (the first whitespace-delimited token
 *                   of the request line).
 * @returns Cost expressed in game time units (>= 0).
 */
[[nodiscard]] int costOf(std::string_view opcode);

}  // namespace zappy::server
