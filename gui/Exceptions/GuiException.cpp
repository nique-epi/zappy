/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** GuiException — root exception for all GUI errors
*/

#include "Exceptions/GuiException.hpp"

namespace zappy::gui {

GuiException::GuiException(const std::string& message)
    : std::runtime_error(message) {}

}  // namespace zappy::gui
