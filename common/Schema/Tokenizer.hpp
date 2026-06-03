/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Tokenizer — splits a protocol line into a vector of string tokens
*/

#pragma once

#include <string>
#include <vector>

namespace zappy::schema {

/**
 * @brief Split a protocol line into tokens.
 *
 * Grammar:
 *   LINE   = *(SP TOKEN)
 *   TOKEN  = QUOTED | BARE
 *   QUOTED = '"' *( CHAR | '\"' ) '"'
 *   BARE   = 1*( any non-space character )
 *
 * @param line Raw string with no trailing newline.
 * @returns Ordered list of unquoted token values.
 */
std::vector<std::string> tokenize(const std::string &line);

}  // namespace zappy::schema
