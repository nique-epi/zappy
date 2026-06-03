/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit tests for zappy::rpc::RPCClient driven through the inject constructor
*/

#include <arpa/inet.h>
#include <gtest/gtest.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include "MockSocket.hpp"
#include "Rpc/Client/RPCClient.hpp"
#include "Rpc/Message/IMessage.hpp"
#include "Rpc/Message/RPCMessage.hpp"
#include "Schema/Fields/StringFieldType.hpp"
#include "Schema/Schema.hpp"

using zappy::network::test::MockSocket;
using zappy::rpc::IMessage;
using zappy::rpc::RPCClient;
using zappy::rpc::RPCMessage;

namespace {

constexpr int noStdin = -1;
constexpr int serverFd = 7;

struct EchoArgs {
  std::string text;
};

std::shared_ptr<const zappy::schema::Schema<EchoArgs>> makeEchoSchema() {
  zappy::schema::Schema<EchoArgs> schema;
  schema.field<zappy::schema::StringFieldType>("text", &EchoArgs::text);
  return std::make_shared<const zappy::schema::Schema<EchoArgs>>(
      std::move(schema));
}

/**
 * @brief RAII loopback listener that accepts one client connection.
 */
class Listener {
 public:
  Listener() {
    fd_ = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
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
  int accept() { return ::accept(fd_, nullptr, nullptr); }

 private:
  int fd_;
  int port_;
};

}  // namespace

TEST(RPCClient, SendEnqueuesLineFlushedOnPollOut) {
  MockSocket mock;
  RPCClient client(noStdin, serverFd, mock);

  client.send("PING");
  mock.pushPollOut(serverFd);
  client.runOnce();

  EXPECT_EQ(mock.sentTo(serverFd), "PING\n");
}

TEST(RPCClient, CallBuildsWireLineFromArguments) {
  auto schema = makeEchoSchema();
  const auto echo = RPCMessage{"ech"}.schema(schema);

  MockSocket mock;
  RPCClient client(noStdin, serverFd, mock);

  client.call(echo, {"hello"});
  mock.pushPollOut(serverFd);
  client.runOnce();

  EXPECT_EQ(mock.sentTo(serverFd), "ech hello\n");
}

TEST(RPCClient, ServerLineRoutedToRawHandler) {
  MockSocket mock;
  RPCClient client(noStdin, serverFd, mock);
  std::string seen;
  client.on(std::string("pong"),
            [&](IMessage &message) { seen = message.opcode(); });

  mock.pushPollIn(serverFd);
  mock.pushReceive(serverFd, "pong\n");
  client.runOnce();

  EXPECT_EQ(seen, "pong");
}

TEST(RPCClient, ServerLineRoutedToTypedHandler) {
  auto schema = makeEchoSchema();
  const auto echo = RPCMessage{"ech"}.schema(schema);

  MockSocket mock;
  RPCClient client(noStdin, serverFd, mock);
  std::string text;
  client.on(echo, [&](const EchoArgs &args) { text = args.text; });

  mock.pushPollIn(serverFd);
  mock.pushReceive(serverFd, "ech world\n");
  client.runOnce();

  EXPECT_EQ(text, "world");
}

TEST(RPCClient, UnknownServerLineRoutedToUnknownHandler) {
  MockSocket mock;
  RPCClient client(noStdin, serverFd, mock);
  std::string seen;
  client.onUnknown([&](IMessage &message) { seen = message.opcode(); });

  mock.pushPollIn(serverFd);
  mock.pushReceive(serverFd, "surprise\n");
  client.runOnce();

  EXPECT_EQ(seen, "surprise");
}

TEST(RPCClient, ConnectsAndSendsOverRealLoopback) {
  Listener listener;
  RPCClient client("127.0.0.1", listener.port(), false);
  int accepted = listener.accept();
  ASSERT_GE(accepted, 0);

  client.onStdin([](const std::string &) {});
  client.send("HELLO");
  client.runOnce(100);

  char buffer[16] = {};
  ssize_t received = recv(accepted, buffer, sizeof(buffer), 0);
  EXPECT_EQ(std::string(buffer, received > 0 ? received : 0), "HELLO\n");
  close(accepted);
}
