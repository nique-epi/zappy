/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** CommandSender — formats and sends GUI -> server protocol commands
*/

#pragma once

#include "Network/INetworkClient.hpp"

namespace zappy::gui {

/**
 * @brief Formats and sends the GUI -> server query commands.
 *
 * Wraps an @ref INetworkClient and exposes one method per command of the
 * GUI -> server protocol (msz, mct, bct, tna, ppo, plv, pin, sgt, sst).
 * Every method builds the exact command string and forwards it to
 * @ref INetworkClient::sendLine, which appends the trailing newline and
 * queues a non-blocking send.
 */
class CommandSender {
 public:
  explicit CommandSender(INetworkClient& network);
  ~CommandSender() = default;

  CommandSender(const CommandSender&) = delete;
  CommandSender& operator=(const CommandSender&) = delete;
  CommandSender(CommandSender&&) = delete;
  CommandSender& operator=(CommandSender&&) = delete;

  /**
   * @brief Request the map dimensions (`msz`).
   */
  void requestMapSize();

  /**
   * @brief Request the content of every tile on the map (`mct`).
   */
  void requestMapContent();

  /**
   * @brief Request the content of the tile at (@p posX, @p posY) (`bct X
   *        Y`).
   */
  void requestTileContent(int posX, int posY);

  /**
   * @brief Request the list of team names (`tna`).
   */
  void requestTeamNames();

  /**
   * @brief Request the position of player @p playerId (`ppo #n`).
   */
  void requestPlayerPosition(int playerId);

  /**
   * @brief Request the level of player @p playerId (`plv #n`).
   */
  void requestPlayerLevel(int playerId);

  /**
   * @brief Request the inventory of player @p playerId (`pin #n`).
   */
  void requestPlayerInventory(int playerId);

  /**
   * @brief Request the current server time unit (`sgt`).
   */
  void requestTimeUnit();

  /**
   * @brief Set the server time unit to @p timeUnit (`sst T`).
   */
  void setTimeUnit(int timeUnit);

 private:
  INetworkClient& network_;
};

}  // namespace zappy::gui
