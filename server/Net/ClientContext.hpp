/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ClientContext — per-connection state shared by GUI and AI clients
*/

#pragma once

#include <string>
#include "Net/ActionQueue.hpp"

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

  /**
   * @brief Pending command lines for AI sessions, bounded at
   *        @ref maxPendingActions.
   *
   * Unused for GUI sessions: GUI commands have no action cost and dispatch
   * immediately.
   */
  ActionQueue pendingActions;

  /**
   * @brief Whether an AI action is currently executing for this session.
   *
   * Set to true while the scheduler holds a deadline callback for the player;
   * cleared when the callback runs and the next pending action is drained.
   */
  bool actionInFlight{false};
};

}  // namespace zappy::server
