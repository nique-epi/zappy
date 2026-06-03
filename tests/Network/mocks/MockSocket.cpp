/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Queue-based mock I/O backend for deterministic network unit tests
*/

#include "MockSocket.hpp"
#include <poll.h>
#include <algorithm>
#include <cerrno>
#include <cstring>

namespace zappy::network::test {

void MockSocket::pushPoll(std::vector<std::pair<int, short>> fired) {
  pollQueue_.push_back({false, 0, std::move(fired)});
}

void MockSocket::pushPollIn(int fd) { pushPoll({{fd, POLLIN}}); }

void MockSocket::pushPollOut(int fd) { pushPoll({{fd, POLLOUT}}); }

void MockSocket::pushAccept(int newFd) { acceptQueue_.push_back(newFd); }

void MockSocket::pushReceive(int fd, const std::string &data) {
  receiveQueue_[fd].push_back(data);
}

void MockSocket::pushDisconnect(int fd) { receiveQueue_[fd].push_back(""); }

void MockSocket::pushPollError(int errnoValue) {
  pollQueue_.push_back({true, errnoValue, {}});
}

void MockSocket::failNextSend(int fd) { sendFailures_[fd]++; }

const std::string &MockSocket::sentTo(int fd) const {
  static const std::string empty;
  auto it = sent_.find(fd);
  return it != sent_.end() ? it->second : empty;
}

int MockSocket::acceptCallCount() const { return acceptCalls_; }

int MockSocket::doPoll(struct pollfd *fds, nfds_t nfds, int) {
  if (pollQueue_.empty()) {
    return 0;
  }
  PollResult result = pollQueue_.front();
  pollQueue_.pop_front();
  if (result.error) {
    errno = result.errnoValue;
    return -1;
  }
  const std::vector<std::pair<int, short>> &fired = result.fired;

  int ready = 0;
  for (nfds_t i = 0; i < nfds; i++) {
    fds[i].revents = 0;
    for (auto &[fd, events] : fired) {
      if (fds[i].fd == fd) {
        fds[i].revents = events;
        ready++;
      }
    }
  }
  return ready;
}

int MockSocket::doAccept(int, struct sockaddr *, socklen_t *) {
  acceptCalls_++;
  if (acceptQueue_.empty()) {
    return -1;
  }
  int fd = acceptQueue_.front();
  acceptQueue_.pop_front();
  return fd;
}

ssize_t MockSocket::doReceive(int fd, void *buffer, size_t length, int) {
  auto it = receiveQueue_.find(fd);
  if (it == receiveQueue_.end() || it->second.empty()) {
    return -1;
  }
  std::string data = it->second.front();
  it->second.pop_front();
  if (data.empty()) {
    return 0;
  }
  size_t count = std::min(length, data.size());
  std::memcpy(buffer, data.c_str(), count);
  return static_cast<ssize_t>(count);
}

ssize_t MockSocket::doSend(int fd, const void *buffer, size_t length, int) {
  auto failure = sendFailures_.find(fd);
  if (failure != sendFailures_.end() && failure->second > 0) {
    failure->second--;
    return -1;
  }
  sent_[fd].append(static_cast<const char *>(buffer), length);
  return static_cast<ssize_t>(length);
}

}  // namespace zappy::network::test
