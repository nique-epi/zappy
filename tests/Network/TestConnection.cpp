/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for zappy::network::Connection (client-side RAII connect)
*/

#include <arpa/inet.h>
#include <gtest/gtest.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdint>
#include <utility>
#include "Network/Client/Connection.hpp"
#include "Network/Exceptions.hpp"

using zappy::network::ConnectError;
using zappy::network::Connection;

namespace {

/**
 * @brief RAII loopback listener bound to 127.0.0.1 on an ephemeral port.
 */
class Listener {
 public:
  Listener() {
    fd_ = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    address.sin_port = 0;
    bind(fd_, reinterpret_cast<struct sockaddr *>(&address), sizeof(address));
    socklen_t length = sizeof(address);
    getsockname(fd_, reinterpret_cast<struct sockaddr *>(&address), &length);
    port_ = ntohs(address.sin_port);
    listen(fd_, 16);
  }

  ~Listener() { close(fd_); }

  Listener(const Listener &) = delete;
  Listener &operator=(const Listener &) = delete;

  int port() const { return port_; }

 private:
  int fd_;
  int port_;
};

}  // namespace

TEST(Connection, ConnectsToListeningServer) {
  Listener listener;
  Connection connection("127.0.0.1", listener.port());
  EXPECT_GE(connection.fd(), 0);
}

TEST(Connection, MoveTransfersOwnership) {
  Listener listener;
  Connection source("127.0.0.1", listener.port());
  int fd = source.fd();
  Connection moved(std::move(source));
  EXPECT_EQ(moved.fd(), fd);
  EXPECT_EQ(source.fd(), -1);
}

TEST(Connection, MoveAssignTransfersOwnership) {
  Listener listener;
  Connection source("127.0.0.1", listener.port());
  Connection target("127.0.0.1", listener.port());
  int fd = source.fd();
  target = std::move(source);
  EXPECT_EQ(target.fd(), fd);
  EXPECT_EQ(source.fd(), -1);
}

TEST(Connection, InvalidAddressThrowsConnectError) {
  EXPECT_THROW(Connection("not-an-ip", 4242), ConnectError);
}

TEST(Connection, RefusedPortThrowsConnectError) {
  int port = 0;
  {
    Listener listener;
    port = listener.port();
  }
  EXPECT_THROW(Connection("127.0.0.1", port), ConnectError);
}
