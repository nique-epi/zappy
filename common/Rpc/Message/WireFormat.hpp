/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** WireFormat — builds a protocol line from an opcode and argument values
*/

#pragma once

#include <string>
#include <vector>

namespace zappy::rpc {

/**
 * @brief Build a wire-format line from an opcode and ordered argument values.
 *
 * Tokens that are empty or contain a space or a quote are double-quoted, with
 * inner quotes escaped. No trailing newline is appended.
 *
 * @param opcode Message opcode placed first on the line.
 * @param args Ordered argument values.
 * @returns The encoded line.
 */
std::string buildWireLine(const std::string &opcode,
                          const std::vector<std::string> &args);

}  // namespace zappy::rpc
