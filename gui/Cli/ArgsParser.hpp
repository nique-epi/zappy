/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ArgsParser — turns argv into a GuiConfig
*/

#pragma once

#include "GuiConfig.hpp"

namespace zappy::gui {

/**
 * @brief Parse the GUI command line into a GuiConfig.
 *
 * Recognises -p (TCP port, integer in [1, 65535]) and -h (server hostname,
 * non-empty string). Both flags are mandatory. Every failure surfaces as a
 * @ref zappy::cli::ParserException subclass; no standard-library exception
 * escapes.
 *
 * @param argumentCount Number of entries in @p arguments (argc).
 * @param arguments     Argument vector (argv).
 * @returns The populated configuration.
 * @throws zappy::cli::UnknownOptionException On an unrecognised flag.
 * @throws zappy::cli::MissingValueException  When a required flag or its
 *         value is absent.
 * @throws zappy::cli::InvalidValueException  When the port is not a valid
 *         in-range integer.
 */
GuiConfig parseArguments(int argumentCount, char** arguments);

}  // namespace zappy::gui
