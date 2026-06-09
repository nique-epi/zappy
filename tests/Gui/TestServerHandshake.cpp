/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Unit and functional tests for ServerHandshake
*/

#include <gtest/gtest.h>
#include <stdexcept>
#include <string>
#include <vector>
#include "Exceptions/HandshakeException.hpp"
#include "Network/INetworkClient.hpp"
#include "Network/ServerHandshake.hpp"

namespace {

class FakeNetwork : public zappy::gui::INetworkClient {
 public:
  void setResponseHandler(ResponseHandler handler) override {
    handler_ = std::move(handler);
  }

  void sendLine(const std::string& line) override { sent_.push_back(line); }

  void inject(const std::string& line) {
    if (handler_) {
      handler_(line);
    }
  }

  [[nodiscard]] const std::vector<std::string>& sent() const { return sent_; }

 private:
  ResponseHandler handler_;
  std::vector<std::string> sent_;
};

}  // namespace

TEST(ServerHandshake, StatusIsPendingBeforeWelcome) {
  FakeNetwork network;
  zappy::gui::ServerHandshake handshake(network);

  EXPECT_EQ(handshake.status(), zappy::gui::HandshakeStatus::Pending);
}

TEST(ServerHandshake, WelcomeCompletesHandshake) {
  FakeNetwork network;
  zappy::gui::ServerHandshake handshake(network);

  network.inject("WELCOME");

  EXPECT_EQ(handshake.status(), zappy::gui::HandshakeStatus::Done);
}

TEST(ServerHandshake, GraphicSentAfterWelcome) {
  FakeNetwork network;
  zappy::gui::ServerHandshake handshake(network);

  network.inject("WELCOME");

  ASSERT_FALSE(network.sent().empty());
  EXPECT_EQ(network.sent().front(), "GRAPHIC");
}

TEST(ServerHandshake, InitialCommandsSentAfterWelcome) {
  FakeNetwork network;
  zappy::gui::ServerHandshake handshake(network);

  network.inject("WELCOME");

  const auto& sent = network.sent();
  ASSERT_EQ(sent.size(), 4U);
  EXPECT_EQ(sent[0], "GRAPHIC");
  EXPECT_EQ(sent[1], "msz");
  EXPECT_EQ(sent[2], "mct");
  EXPECT_EQ(sent[3], "tna");
}

TEST(ServerHandshake, UnrelatedMessageDoesNotCompleteHandshake) {
  FakeNetwork network;
  zappy::gui::ServerHandshake handshake(network);

  network.inject("WHATEVER");

  EXPECT_EQ(handshake.status(), zappy::gui::HandshakeStatus::Pending);
  EXPECT_TRUE(network.sent().empty());
}

TEST(ServerHandshake, OnlyWelcomeCompletesNotCaseSensitive) {
  FakeNetwork network;
  zappy::gui::ServerHandshake handshake(network);

  network.inject("welcome");
  network.inject("Welcome");

  EXPECT_EQ(handshake.status(), zappy::gui::HandshakeStatus::Pending);
}

TEST(ServerHandshake, CheckTimeoutIsNoOpWhenDone) {
  FakeNetwork network;
  zappy::gui::ServerHandshake handshake(network, 1);

  network.inject("WELCOME");

  EXPECT_NO_THROW(handshake.checkTimeout());
}

TEST(ServerHandshake, TimeoutThrowsHandshakeTimeoutException) {
  FakeNetwork network;
  zappy::gui::ServerHandshake handshake(network, 0);

  EXPECT_THROW(handshake.checkTimeout(), zappy::gui::HandshakeTimeoutException);
}

TEST(ServerHandshake, TimeoutExceptionIsAHandshakeException) {
  FakeNetwork network;
  zappy::gui::ServerHandshake handshake(network, 0);

  EXPECT_THROW(handshake.checkTimeout(), zappy::gui::HandshakeException);
}

TEST(ServerHandshake, HandshakeExceptionIsAStdRuntimeError) {
  EXPECT_THROW(throw zappy::gui::HandshakeTimeoutException(),
               std::runtime_error);
}

TEST(ServerHandshake, HandshakeExceptionMessageIsPreserved) {
  const std::string msg = "server did not send WELCOME within the timeout";
  EXPECT_EQ(std::string(zappy::gui::HandshakeTimeoutException().what()), msg);
}

TEST(ServerHandshake, SecondWelcomeIsIgnored) {
  FakeNetwork network;
  zappy::gui::ServerHandshake handshake(network);

  network.inject("WELCOME");
  const std::size_t sentAfterFirst = network.sent().size();

  network.inject("WELCOME");

  EXPECT_EQ(network.sent().size(), sentAfterFirst);
}
