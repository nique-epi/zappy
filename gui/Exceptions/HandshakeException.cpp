/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** HandshakeException — exception hierarchy for the GUI server handshake
*/

#include "Exceptions/HandshakeException.hpp"
#include "Exceptions/HandshakeMessages.hpp"

namespace zappy::gui {

namespace msg = zappy::gui::messages;

HandshakeException::HandshakeException(const std::string& message)
    : GuiException(message) {}

HandshakeTimeoutException::HandshakeTimeoutException()
    : HandshakeException(msg::HANDSHAKE_TIMEOUT) {}

}  // namespace zappy::gui
