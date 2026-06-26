/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** UsageHelp — the --help facility of a schema-driven command-line parser
*/

#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include "Cli/OptionSchema.hpp"

namespace zappy::cli {

/**
 * @brief The `--help` behaviour of one binary's command line.
 *
 * Bundles the two operations that share a binary's option schema: deciding
 * whether the command line asks for the usage, and rendering that usage text.
 * Build one from the binary's synopsis and option schema, then query it.
 *
 * @tparam Config The configuration struct the option schema populates.
 */
template <typename Config>
class UsageHelp {
 public:
  /**
   * @brief Build the help facility for a binary.
   *
   * @param synopsis The option synopsis shown after `USAGE: ` (e.g.
   *                 `./zappy_server -p port ...`).
   * @param options  The recognised option schema, reused for both the short
   *                 `-h` reservation check and the rendered option table.
   */
  UsageHelp(std::string synopsis, std::vector<OptionSchema<Config>> options);

  /**
   * @brief Whether @p arguments ask for the usage message.
   *
   * `--help` always matches. The short `-h` only matches when the schema does
   * not already use `-h` for one of its own options (the GUI binds it to the
   * server hostname). The scan never throws, so it is safe to run before the
   * parser on an otherwise invalid command line.
   *
   * @param argumentCount Number of entries in @p arguments (argc).
   * @param arguments     Argument vector (argv).
   * @returns True when the usage was requested.
   */
  [[nodiscard]] bool isRequested(int argumentCount, char** arguments) const;

  /**
   * @brief The rendered usage text, without a trailing newline.
   *
   * The `USAGE:` synopsis line followed by one aligned row per option, each
   * built from the flag, its field-type name as a metavariable, and its
   * description.
   *
   * @returns The formatted usage block.
   */
  [[nodiscard]] std::string text() const;

 private:
  [[nodiscard]] bool reservesShortFlag() const;

  static constexpr const char* longFlag = "--help";
  static constexpr const char* shortFlag = "-h";
  static constexpr const char* rowIndent = "    ";
  static constexpr std::size_t rowColumnGap = 2;

  std::string synopsis_;
  std::vector<OptionSchema<Config>> options_;
};

}  // namespace zappy::cli

#include "Cli/UsageHelp.tpp"
