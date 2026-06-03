/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Network exception hierarchy
*/

#include "Network/Exceptions.hpp"

namespace zappy::network {

NetworkError::NetworkError(const std::string &message)
    : std::runtime_error(message) {}

SocketError::SocketError(const std::string &message) : NetworkError(message) {}

BindError::BindError(const std::string &message) : NetworkError(message) {}

ListenError::ListenError(const std::string &message) : NetworkError(message) {}

ConnectError::ConnectError(const std::string &message)
    : NetworkError(message) {}

AcceptError::AcceptError(const std::string &message) : NetworkError(message) {}

PollError::PollError(const std::string &message) : NetworkError(message) {}

}  // namespace zappy::network
