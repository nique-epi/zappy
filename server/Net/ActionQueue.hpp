/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ActionQueue — bounded FIFO of pending AI command lines for one session
*/

#pragma once

#include <cstddef>
#include <deque>
#include <optional>
#include <string>

namespace zappy::server {

/// Per-session cap on pending AI commands (subject mandate).
constexpr std::size_t maxPendingActions = 10;

/**
 * @brief Bounded FIFO buffering an AI client's pending command lines.
 *
 * The Zappy subject caps each AI client at ten pending requests; anything sent
 * beyond that cap is silently dropped, never reordered. This queue stores the
 * raw protocol lines (without the trailing newline) in arrival order and lets
 * the action pipeline pop them one at a time when the player's current action
 * completes.
 *
 * The queue is purely a storage primitive: it owns no timing, no dispatch and
 * no notion of "busy". Those concerns live in GameServer's AI pipeline.
 */
class ActionQueue {
 public:
  /**
   * @brief Append @p line at the back of the queue.
   * @param[in] line Raw protocol line (no trailing newline).
   * @returns true on success, false when the queue already holds
   *          @ref maxPendingActions entries.
   */
  bool push(const std::string& line);

  /**
   * @brief Remove and return the front-most line.
   * @returns The oldest queued line, or std::nullopt when empty.
   */
  std::optional<std::string> pop();

  /// Whether the queue currently holds no line.
  [[nodiscard]] bool empty() const;

  /// Whether the queue holds @ref maxPendingActions entries.
  [[nodiscard]] bool full() const;

  /// Number of lines currently queued.
  [[nodiscard]] std::size_t size() const;

  /// Drop every queued line.
  void clear();

 private:
  std::deque<std::string> lines_;
};

}  // namespace zappy::server
