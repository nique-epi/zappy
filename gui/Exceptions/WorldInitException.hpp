/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** WorldInitException — exception hierarchy for the GUI world initialization
*/

#pragma once

#include <string>
#include "Exceptions/GuiException.hpp"

namespace zappy::gui {

/**
 * @brief Root exception for all world initialization errors.
 *
 * Every failure of the msz / mct / tna startup sequence surfaces as a
 * subclass of @ref WorldInitException so the caller can catch the whole
 * family at once.
 */
class WorldInitException : public GuiException {
 public:
  explicit WorldInitException(const std::string& message);
};

/**
 * @brief Thrown when the server does not complete the msz / mct / tna
 *        startup sequence within the configured timeout.
 */
class WorldInitTimeoutException : public WorldInitException {
 public:
  WorldInitTimeoutException();
};

}  // namespace zappy::gui
