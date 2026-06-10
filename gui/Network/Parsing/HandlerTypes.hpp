/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** HandlerTypes — type alias for the server message dispatch table
*/

#pragma once

#include <functional>
#include <sstream>
#include <string>
#include <unordered_map>

namespace zappy::gui {

using HandlerMap =
    std::unordered_map<std::string, std::function<void(std::istringstream&)>>;

}  // namespace zappy::gui
