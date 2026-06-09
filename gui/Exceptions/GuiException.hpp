/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** GuiException — root exception for all GUI errors
*/

#pragma once

#include <stdexcept>
#include <string>

namespace zappy::gui {

/**
 * @brief Root exception for every error raised by the GUI.
 *
 * All GUI exception families (parser, handshake, …) inherit from this class
 * so that callers can catch the whole GUI error surface at once.
 */
class GuiException : public std::runtime_error {
 public:
  explicit GuiException(const std::string& message);
};

}  // namespace zappy::gui
