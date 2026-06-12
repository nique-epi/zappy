/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** FakeNetwork — in-memory INetworkClient test double
*/

#pragma once

#include <string>
#include <vector>
#include "Network/INetworkClient.hpp"

namespace zappy::gui::test {

/**
 * @brief In-memory INetworkClient recording sent lines and replaying
 *        injected ones.
 *
 * @ref sendLine appends to an internal buffer inspectable via @ref sent.
 * @ref inject invokes the registered response handler as if the line had
 * been received from the server, for tests exercising response-driven
 * logic (e.g. the handshake state machine).
 */
class FakeNetwork : public INetworkClient {
 public:
  void setResponseHandler(ResponseHandler handler) override;
  void sendLine(const std::string& line) override;

  /**
   * @brief Simulate a line received from the server.
   */
  void inject(const std::string& line);

  [[nodiscard]] const std::vector<std::string>& sent() const;

 private:
  ResponseHandler handler_;
  std::vector<std::string> sent_;
};

}  // namespace zappy::gui::test
