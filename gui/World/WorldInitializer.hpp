/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** WorldInitializer — sequential msz/mct/tna startup sequence
*/

#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include "Network/INetworkClient.hpp"
#include "Network/Parsing/MessageParser.hpp"
#include "World/WorldState.hpp"

namespace zappy::gui {

/**
 * @brief Step of the sequential world initialization sequence.
 */
enum class WorldInitPhase : std::uint8_t {
  WaitingMapSize,
  WaitingMapContent,
  WaitingTeamNames,
  Done
};

/**
 * @brief Drives the sequential msz / mct / tna exchange that populates a
 *        WorldState at GUI startup.
 *
 * Installs itself as the @ref INetworkClient response handler on
 * construction and immediately requests the map size (@c msz). Every line
 * received from the server is forwarded to the supplied @ref MessageParser,
 * which applies the corresponding @ref WorldState update; the initializer
 * itself only tracks when to advance to the next request.
 *
 * Once the map size is known, the map content (@c mct) is requested unless
 * the map is empty, then the team names (@c tna).
 *
 * The caller must invoke @ref onPollRoundComplete after every network poll
 * and @ref checkTimeout on every poll until @ref isDone returns true.
 */
class WorldInitializer {
 public:
  static constexpr int DEFAULT_TIMEOUT_MS = 10000;

  /**
   * @brief Install the initialization handler on @p network and request the
   *        map size.
   *
   * @param network   Connection to the server (ownership not transferred).
   * @param parser    Parser applying server lines to @p world.
   * @param world     WorldState populated by @p parser as responses arrive.
   * @param timeoutMs Maximum milliseconds to complete the sequence before
   *                  @ref checkTimeout throws @ref WorldInitTimeoutException.
   */
  explicit WorldInitializer(INetworkClient& network, MessageParser& parser,
                            WorldState& world,
                            int timeoutMs = DEFAULT_TIMEOUT_MS);

  /**
   * @brief Clear the response handler installed on @p network.
   */
  ~WorldInitializer();

  WorldInitializer(const WorldInitializer&) = delete;
  WorldInitializer& operator=(const WorldInitializer&) = delete;
  WorldInitializer(WorldInitializer&&) = delete;
  WorldInitializer& operator=(WorldInitializer&&) = delete;

  /**
   * @brief Return true once the team names have all been received.
   */
  [[nodiscard]] bool isDone() const;

  /**
   * @brief Throw @ref WorldInitTimeoutException if the sequence is still
   *        running and the deadline has been exceeded.
   *
   * Must be called once per poll until @ref isDone returns true.
   */
  void checkTimeout() const;

  /**
   * @brief Notify the initializer that one network poll round has finished.
   *
   * Used to detect the end of the @c tna burst: the team-name phase
   * completes once a poll round elapses without receiving a new @c tna line.
   */
  void onPollRoundComplete();

 private:
  void onServerLine(const std::string& line);
  void handleMapSizeToken(const std::string& token);
  void handleTileContentToken(const std::string& token);
  void handleTeamNameToken(const std::string& token);

  INetworkClient& network_;
  MessageParser& parser_;
  WorldState& world_;
  WorldInitPhase phase_{WorldInitPhase::WaitingMapSize};
  int expectedTileCount_{0};
  int receivedTileCount_{0};
  bool receivedAnyTeamName_{false};
  bool receivedTeamNameThisRound_{false};
  std::chrono::steady_clock::time_point deadline_;
};

}  // namespace zappy::gui
