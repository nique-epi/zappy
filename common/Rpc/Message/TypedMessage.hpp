/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** TypedMessage<T> — RPCMessage bound to a Schema for typed handler dispatch
*/

#pragma once

#include <memory>
#include <string>
#include <vector>
#include "Rpc/Message/IMessage.hpp"
#include "Rpc/Message/WireFormat.hpp"
#include "Schema/Schema.hpp"

namespace zappy::rpc {

/**
 * @brief An RPC message bound to a Schema<T>.
 *
 * Produced by RPCMessage::schema(Schema<T>) — never constructed directly.
 * Holds an opcode and shared ownership of the Schema<T> that validates and
 * populates T from a protocol line. Owning the schema (rather than referencing
 * it) means a TypedMessage stays valid for its whole lifetime regardless of
 * where the originating schema was declared. Pass it to RPCServer::on() to get
 * a typed handler.
 */
template <typename T>
class TypedMessage {
 public:
  TypedMessage(std::string opcode,
               std::shared_ptr<const schema::Schema<T>> schema)
      : opcode_(std::move(opcode)), schema_(std::move(schema)) {}

  const std::string &opcode() const { return opcode_; }
  const schema::Schema<T> &schema() const { return *schema_; }
  const std::shared_ptr<const schema::Schema<T>> &schemaPtr() const {
    return schema_;
  }

  /**
   * @brief Parse and validate args from an already-dispatched AMessage.
   */
  schema::ParseResult<T> parse(const AMessage &message) const {
    return schema_->parse(message.args());
  }

  /**
   * @brief Build a wire-format line from ordered argument values.
   */
  std::string buildLine(const std::vector<std::string> &args) const {
    return buildWireLine(opcode_, args);
  }

 private:
  std::string opcode_;
  std::shared_ptr<const schema::Schema<T>> schema_;
};

}  // namespace zappy::rpc
