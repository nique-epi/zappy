/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ArgumentCursor — forward cursor over argv with value extraction
*/

#pragma once

#include <string>

namespace zappy::server {

/**
 * @brief Forward, single-pass cursor over the argument vector (argv).
 *
 * The cursor starts positioned on the program name and walks the remaining
 * tokens left to right. An option applier consumes its own values through
 * @ref requireValue (exactly one value) or @ref nextIsValue / @ref nextToken
 * (zero or more values up to the next option flag).
 */
class ArgumentCursor {
 public:
  /**
   * @brief Build a cursor over @p arguments.
   *
   * @param argumentCount Number of entries in @p arguments (argc).
   * @param arguments     Argument vector (argv).
   */
  ArgumentCursor(int argumentCount, char **arguments);

  /**
   * @brief Whether at least one unread token remains.
   */
  [[nodiscard]] bool hasNextToken() const;

  /**
   * @brief Advance to and return the next token.
   *
   * @pre @ref hasNextToken returns true.
   */
  std::string nextToken();

  /**
   * @brief Whether the next token exists and is a value (not an option flag).
   */
  [[nodiscard]] bool nextIsValue() const;

  /**
   * @brief Consume and return the value following an option flag.
   *
   * @param option The flag requiring the value, used for the error message.
   * @returns The next token.
   * @throws MissingValueException If no value token follows @p option.
   */
  std::string requireValue(const std::string &option);

 private:
  int argumentCount_;
  char **arguments_;
  int index_;
};

}  // namespace zappy::server
