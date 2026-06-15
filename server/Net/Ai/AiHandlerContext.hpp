/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** AiHandlerContext — world dependencies shared by the AI command handlers
*/

#pragma once

namespace zappy::world {
class PlayerRegistry;
class Map;
class TeamRegistry;
}  // namespace zappy::world

namespace zappy::server {

/**
 * @brief References to the world state the AI command handlers operate on.
 *
 * Passed to every `install*Handlers` installer so the handler closures share a
 * single bundle of dependencies instead of capturing them one by one. The
 * referenced objects are owned by the @ref GameServer and outlive the closures
 * registered on its server.
 */
struct AiHandlerContext {
  world::PlayerRegistry& players;
  world::Map& map;
  world::TeamRegistry& teams;
};

}  // namespace zappy::server
