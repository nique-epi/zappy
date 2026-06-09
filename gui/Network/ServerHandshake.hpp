/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ServerHandshake — handshake state machine for the Zappy server protocol
*/

#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include "Network/INetworkClient.hpp"

namespace zappy::gui {

/**
 * @brief State of the server handshake sequence.
 */
enum class HandshakeStatus : std::uint8_t { Pending, Done };

/**
 * @brief Drives the WELCOME / GRAPHIC handshake with the Zappy server.
 *
 * Installs itself as the @ref INetworkClient response handler on construction.
 * Waits for the server to send @c WELCOME, then replies with @c GRAPHIC and
 * enqueues the three initial GUI commands (@c msz, @c mct, @c tna).
 *
 * The caller must invoke @ref checkTimeout on every render frame to detect a
 * server that stalls before sending @c WELCOME.
 */
class ServerHandshake {
 public:
  static constexpr int DEFAULT_TIMEOUT_MS = 10000;

  /**
   * @brief Install the handshake handler on @p network.
   *
   * @param network    Connection to the server (ownership not transferred).
   * @param timeoutMs  Maximum milliseconds to wait for @c WELCOME before
   *                   @ref checkTimeout throws @ref HandshakeTimeoutException.
   */
  explicit ServerHandshake(INetworkClient& network,
                           int timeoutMs = DEFAULT_TIMEOUT_MS);
  ~ServerHandshake() = default;
  ServerHandshake(const ServerHandshake&) = delete;
  ServerHandshake& operator=(const ServerHandshake&) = delete;
  ServerHandshake(ServerHandshake&&) = delete;
  ServerHandshake& operator=(ServerHandshake&&) = delete;

  /**
   * @brief Return the current handshake state.
   */
  [[nodiscard]] HandshakeStatus status() const;

  /**
   * @brief Throw @ref HandshakeTimeoutException if the handshake is still
   *        Pending and the deadline has been exceeded.
   *
   * Must be called once per render frame until @ref status returns Done.
   */
  void checkTimeout() const;

 private:
  void onServerLine(const std::string& line);
  void sendInitialCommands();

  INetworkClient& network_;
  HandshakeStatus status_{HandshakeStatus::Pending};
  std::chrono::steady_clock::time_point deadline_;
};

}  // namespace zappy::gui
