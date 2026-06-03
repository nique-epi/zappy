/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ClientContext — per-connection state shared by GUI and AI clients
*/

#pragma once

#include <string>

namespace zappy::server {

/**
 * @brief Kind of client behind a connection, resolved at the handshake.
 *
 * A connection starts Pending; the team name sent after WELCOME promotes it to
 * Gui (team name "GRAPHIC") or Ai (any other team name).
 */
enum class ClientType { Pending, Gui, Ai };

/**
 * @brief Per-connection state attached to every session.
 *
 * Populated during the handshake; carries no game state yet (protocol shell).
 */
struct ClientContext {
  ClientType type{ClientType::Pending};
  std::string teamName;
};

}  // namespace zappy::server
