/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ConnectionBootstrap — handshake then world initialization over the network
*/

#pragma once

namespace zappy::gui {

class NetworkManager;
class MessageParser;
struct WorldState;

/**
 * @brief Run the WELCOME handshake then the world-initialization exchange.
 *
 * Drives the polling loop until both phases complete, then installs @p parser
 * as the network's response handler for the rest of the session.
 *
 * @param network Connected client to poll and to register the handler on.
 * @param world   World filled in by the initialization exchange.
 * @param parser  Parser fed with every subsequent server line.
 * @returns false if the window was closed before completion, true otherwise.
 */
bool handshakeAndInitializeWorld(NetworkManager& network, WorldState& world,
                                 MessageParser& parser);

}  // namespace zappy::gui
