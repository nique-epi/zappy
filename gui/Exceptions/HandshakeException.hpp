/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** HandshakeException — exception hierarchy for the GUI server handshake
*/

#pragma once

#include <string>
#include "Exceptions/GuiException.hpp"

namespace zappy::gui {

/**
 * @brief Root exception for all handshake errors.
 *
 * The GUI never lets a lower-level exception escape the handshake sequence:
 * every failure surfaces as a subclass of @ref HandshakeException so the
 * caller can catch the whole family at once.
 */
class HandshakeException : public GuiException {
 public:
  explicit HandshakeException(const std::string& message);
};

/**
 * @brief Thrown when the server does not send WELCOME within the configured
 *        timeout.
 */
class HandshakeTimeoutException : public HandshakeException {
 public:
  HandshakeTimeoutException();
};

}  // namespace zappy::gui
