/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Network exception hierarchy
*/

#pragma once

#include <stdexcept>
#include <string>

namespace zappy::network {

/**
 * @brief Root exception for all network-layer errors.
 */
class NetworkError : public std::runtime_error {
 public:
  explicit NetworkError(const std::string &message);
};

/**
 * @brief Thrown when socket creation or setsockopt fails.
 */
class SocketError : public NetworkError {
 public:
  explicit SocketError(const std::string &message);
};

/**
 * @brief Thrown when bind(2) fails.
 */
class BindError : public NetworkError {
 public:
  explicit BindError(const std::string &message);
};

/**
 * @brief Thrown when listen(2) fails.
 */
class ListenError : public NetworkError {
 public:
  explicit ListenError(const std::string &message);
};

/**
 * @brief Thrown when connect(2) fails.
 */
class ConnectError : public NetworkError {
 public:
  explicit ConnectError(const std::string &message);
};

/**
 * @brief Thrown when accept(2) returns -1 on an incoming connection.
 */
class AcceptError : public NetworkError {
 public:
  explicit AcceptError(const std::string &message);
};

/**
 * @brief Thrown when poll(2) returns a fatal error.
 */
class PollError : public NetworkError {
 public:
  explicit PollError(const std::string &message);
};

}  // namespace zappy::network
