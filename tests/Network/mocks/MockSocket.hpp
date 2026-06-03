/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Queue-based mock I/O backend for deterministic network unit tests
*/

#pragma once

#include <cstddef>
#include <deque>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include "Network/Socket/ISocket.hpp"

namespace zappy::network::test {

/**
 * @brief Deterministic, synchronous ISocket implementation for unit tests.
 *
 * Queue events before calling PollLoop::runOnce() and inspect what was sent
 * afterwards. No threads, sleeps, or real file descriptors are involved, so
 * every test is fully reproducible.
 */
class MockSocket : public ISocket {
 public:
  /**
   * @brief Queue one poll() result: the list of (fd, events) pairs that fire.
   * @param fired File descriptors and the revents they should report.
   */
  void pushPoll(std::vector<std::pair<int, short>> fired);

  /**
   * @brief Queue a single POLLIN event on @p fd.
   */
  void pushPollIn(int fd);

  /**
   * @brief Queue a single POLLOUT event on @p fd.
   */
  void pushPollOut(int fd);

  /**
   * @brief Queue an accept() result returning @p newFd.
   */
  void pushAccept(int newFd);

  /**
   * @brief Queue a recv() payload delivered for @p fd.
   */
  void pushReceive(int fd, const std::string &data);

  /**
   * @brief Queue a recv() that returns 0 (clean disconnect) for @p fd.
   */
  void pushDisconnect(int fd);

  /**
   * @brief Queue a poll() that fails: sets @p errnoValue and returns -1.
   */
  void pushPollError(int errnoValue);

  /**
   * @brief Make the next doSend() on @p fd fail once (return -1).
   */
  void failNextSend(int fd);

  /**
   * @brief Return everything sent to @p fd (empty string if nothing sent).
   */
  const std::string &sentTo(int fd) const;

  /**
   * @brief Number of times doAccept() has been called.
   */
  int acceptCallCount() const;

  int doPoll(struct pollfd *fds, nfds_t nfds, int timeout) override;
  int doAccept(int fd, struct sockaddr *address, socklen_t *length) override;
  ssize_t doReceive(int fd, void *buffer, size_t length, int flags) override;
  ssize_t doSend(int fd, const void *buffer, size_t length, int flags) override;

 private:
  /**
   * @brief One queued poll() outcome: either a fired-event list or an error.
   */
  struct PollResult {
    bool error;
    int errnoValue;
    std::vector<std::pair<int, short>> fired;
  };

  std::deque<PollResult> pollQueue_;
  std::deque<int> acceptQueue_;
  std::unordered_map<int, std::deque<std::string>> receiveQueue_;
  std::unordered_map<int, std::string> sent_;
  std::unordered_map<int, int> sendFailures_;
  int acceptCalls_ = 0;
};

}  // namespace zappy::network::test
