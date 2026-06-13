/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** WorldInitException — exception hierarchy for the GUI world initialization
*/

#include "Exceptions/WorldInitException.hpp"
#include "Exceptions/WorldInitMessages.hpp"

namespace zappy::gui {

namespace msg = zappy::gui::messages;

WorldInitException::WorldInitException(const std::string& message)
    : GuiException(message) {}

WorldInitTimeoutException::WorldInitTimeoutException()
    : WorldInitException(msg::WORLD_INIT_TIMEOUT) {}

}  // namespace zappy::gui
