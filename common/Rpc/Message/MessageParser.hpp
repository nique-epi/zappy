/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Protocol line parser — produces a ParsedMessage from a raw line
*/

#pragma once

#include <string>
#include "Rpc/Message/IMessage.hpp"

namespace zappy::rpc {

/**
 * @brief Parse one newline-stripped protocol line into a ParsedMessage.
 *
 * @param line Raw protocol line with the trailing newline already removed.
 * @returns ParsedMessage; opcode is empty if the line is blank.
 */
ParsedMessage parseMessage(const std::string &line);

}  // namespace zappy::rpc
