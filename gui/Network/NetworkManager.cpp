/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** NetworkManager — non-blocking TCP connection to the Zappy server
*/

#include "Network/NetworkManager.hpp"
#include <fcntl.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <format>
#include <memory>
#include <string>
#include <system_error>
#include <utility>
#include "Network/Client/ClientLoop.hpp"  // NOLINT(misc-include-cleaner)
#include "Network/Exceptions.hpp"         // NOLINT(misc-include-cleaner)

namespace zappy::gui {

namespace {

constexpr int connectTimeoutMs = 5000;
constexpr int pollTimeoutSentinel = -1;

constexpr const char* errorResolutionFailed =
    "hostname resolution failed for '{}': {}";
constexpr const char* errorConnectionTimedOut = "connection to {}:{} timed out";
constexpr const char* errorConnectionFailed = "connection to {}:{} failed: {}";
constexpr const char* errorCouldNotConnect = "could not connect to {}:{}: {}";

class AddrInfoGuard {
 public:
  explicit AddrInfoGuard(addrinfo* resolved) : info_(resolved) {}
  ~AddrInfoGuard() {
    if (info_ != nullptr) {
      freeaddrinfo(info_);
    }
  }
  AddrInfoGuard(const AddrInfoGuard&) = delete;
  AddrInfoGuard& operator=(const AddrInfoGuard&) = delete;
  AddrInfoGuard(AddrInfoGuard&& other) noexcept
      : info_(std::exchange(other.info_, nullptr)) {}
  AddrInfoGuard& operator=(AddrInfoGuard&&) = delete;

  [[nodiscard]] addrinfo* get() const { return info_; }

 private:
  addrinfo* info_ = nullptr;
};

int waitForConnectResult(int socketFd) {
  struct pollfd pfd = {.fd = socketFd, .events = POLLOUT, .revents = 0};
  const int ready = poll(&pfd, 1, connectTimeoutMs);
  if (ready == 0) {
    return pollTimeoutSentinel;
  }
  if (ready < 0) {
    return errno;
  }
  int socketError = 0;
  socklen_t errorLength = sizeof(socketError);
  if (getsockopt(socketFd, SOL_SOCKET, SO_ERROR, &socketError, &errorLength) <
      0) {
    return errno;
  }
  return socketError;
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
AddrInfoGuard resolveHostname(const std::string& hostname, int port) {
  addrinfo hints = {};
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  addrinfo* rawInfo = nullptr;
  const std::string portString = std::to_string(port);
  const int status =
      getaddrinfo(hostname.c_str(), portString.c_str(), &hints, &rawInfo);
  AddrInfoGuard guard(rawInfo);
  if (status != 0) {
    throw zappy::network::ConnectError(
        std::format(errorResolutionFailed, hostname, gai_strerror(status)));
  }
  return guard;
}

int makeNonBlockingSocket(const addrinfo& candidate) {
  // NOLINTNEXTLINE(hicpp-vararg)
  const int socketFd =
      socket(candidate.ai_family, candidate.ai_socktype, candidate.ai_protocol);
  if (socketFd < 0) {
    return -1;
  }
  // NOLINTNEXTLINE(hicpp-vararg)
  const int flags = fcntl(socketFd, F_GETFL, 0);
  // NOLINTNEXTLINE(hicpp-vararg,hicpp-signed-bitwise)
  if (flags < 0 || fcntl(socketFd, F_SETFL, flags | O_NONBLOCK) < 0) {
    close(socketFd);
    return -1;
  }
  return socketFd;
}

void applyTcpNoDelay(int socketFd) {
  const int flag = 1;
  setsockopt(socketFd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
void waitForNonBlockingConnect(int socketFd, const std::string& hostname,
                               int port) {
  const int connectError = waitForConnectResult(socketFd);
  if (connectError == pollTimeoutSentinel) {
    close(socketFd);
    throw zappy::network::ConnectError(
        std::format(errorConnectionTimedOut, hostname, port));
  }
  if (connectError != 0) {
    close(socketFd);
    throw zappy::network::ConnectError(
        std::format(errorConnectionFailed, hostname, port,
                    std::generic_category().message(connectError)));
  }
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
int connectCandidate(const addrinfo& candidate, const std::string& hostname,
                     int port) {
  const int socketFd =  // NOLINT(cppcoreguidelines-init-variables)
      makeNonBlockingSocket(candidate);
  if (socketFd < 0) {
    return -1;
  }
  const int connectStatus =
      connect(socketFd, candidate.ai_addr, candidate.ai_addrlen);
  if (connectStatus != 0 && errno != EINPROGRESS) {
    close(socketFd);
    return -1;
  }
  if (connectStatus != 0) {
    waitForNonBlockingConnect(socketFd, hostname, port);
  }
  applyTcpNoDelay(socketFd);
  return socketFd;
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
int connectToServer(const std::string& hostname, int port) {
  const AddrInfoGuard guard = resolveHostname(hostname, port);
  std::string lastError;
  for (const addrinfo* candidate = guard.get(); candidate != nullptr;
       candidate = candidate->ai_next) {
    try {
      const int socketFd =  // NOLINT(cppcoreguidelines-init-variables)
          connectCandidate(*candidate, hostname, port);
      if (socketFd >= 0) {
        return socketFd;
      }
    } catch (const zappy::network::ConnectError& error) {
      lastError = error.what();
    }
  }
  if (!lastError.empty()) {
    throw zappy::network::ConnectError(lastError);
  }
  throw zappy::network::ConnectError(
      std::format(errorCouldNotConnect, hostname, port,
                  std::generic_category().message(errno)));
}

}  // namespace
// NOLINTEND(misc-include-cleaner)

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
NetworkManager::NetworkManager(const std::string& hostname, int port)
    : fd_(connectToServer(hostname, port)),
      io_(),
      loop_(std::make_unique<zappy::network::ClientLoop>(-1, fd_, io_)) {}

NetworkManager::~NetworkManager() {
  if (fd_ >= 0) {
    close(fd_);
  }
}

void NetworkManager::setResponseHandler(ResponseHandler handler) {
  loop_->setResponseHandler(std::move(handler));
}

void NetworkManager::runOnce(int timeoutMs) { loop_->runOnce(timeoutMs); }

void NetworkManager::sendLine(const std::string& line) {
  loop_->sendLine(line);
}

void NetworkManager::stop() { loop_->stop(); }

}  // namespace zappy::gui
