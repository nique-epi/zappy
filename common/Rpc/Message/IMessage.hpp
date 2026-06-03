/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Abstract protocol message instance — opcode + typed argument access
*/

#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace zappy::rpc {

/**
 * @brief Abstract interface for a parsed protocol message.
 */
class IMessage {
 public:
  virtual ~IMessage() = default;

  /**
   * @brief Return the message opcode (e.g. "msz", "pnw").
   */
  virtual const std::string &opcode() const = 0;

  /**
   * @brief Return the argument at position @p index (0-based).
   * @throws std::out_of_range if @p index is past the last argument.
   */
  virtual const std::string &arg(std::size_t index) const = 0;

  /**
   * @brief Return the number of arguments.
   */
  virtual std::size_t argCount() const = 0;
};

/**
 * @brief Abstract base providing common opcode + args storage.
 *
 * Exposes args() for direct vector access used by Schema::parse.
 */
class AMessage : public IMessage {
 public:
  AMessage(std::string opcode, std::vector<std::string> args);

  const std::string &opcode() const override;
  const std::string &arg(std::size_t index) const override;
  std::size_t argCount() const override;

  const std::vector<std::string> &args() const;

 protected:
  explicit AMessage(std::string opcode);

 private:
  std::string opcode_;
  std::vector<std::string> args_;
};

/**
 * @brief Concrete IMessage produced by the protocol parser.
 */
class ParsedMessage : public AMessage {
 public:
  ParsedMessage(std::string opcode, std::vector<std::string> args);
};

}  // namespace zappy::rpc
