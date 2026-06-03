/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** RPCMessage — shared protocol message definition (opcode + wire builder)
*/

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "Rpc/Message/TypedMessage.hpp"
#include "Schema/Schema.hpp"

namespace zappy::rpc {

/**
 * @brief Shared declaration of a protocol message — opcode + wire builder.
 *
 * Use .schema(Schema<T>) to bind a typed struct and get a TypedMessage<T>
 * that validates args and populates T in handlers. Argument-less messages
 * (suc, sbp, …) need no schema.
 */
class RPCMessage {
 public:
  RPCMessage() = default;
  explicit RPCMessage(std::string opcode);

  /**
   * @brief Bind a Schema<T> — returns a TypedMessage<T> with this opcode.
   */
  template <typename T>
  TypedMessage<T> schema(
      std::shared_ptr<const schema::Schema<T>> boundSchema) const {
    return TypedMessage<T>{opcode_, std::move(boundSchema)};
  }

  const std::string &opcode() const;

  /**
   * @brief Build a wire-format line from the given argument values.
   */
  std::string buildLine(const std::vector<std::string> &args) const;

 private:
  std::string opcode_;
};

}  // namespace zappy::rpc
