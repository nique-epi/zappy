/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** ResponseDispatcher — routes server→client lines to registered handlers
*/

#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <utility>
#include "Rpc/Message/IMessage.hpp"
#include "Rpc/Message/RPCMessage.hpp"
#include "Rpc/Message/TypedMessage.hpp"
#include "Schema/Schema.hpp"

namespace zappy::rpc {

/**
 * @brief Routes incoming server→client lines to per-opcode handler functions.
 *
 * The client-side counterpart of Dispatcher: there is no Session, handlers
 * receive only the IMessage. Two registration styles: typed (Schema validates
 * and populates T) or raw (handler receives the IMessage directly). On schema
 * validation failure the typed handler is silently skipped — a client drops a
 * malformed server line rather than replying to it.
 */
class ResponseDispatcher {
 public:
  using Handler = std::function<void(IMessage &)>;

  /**
   * @brief Register a typed handler bound to a TypedMessage<T>.
   */
  template <typename T, typename F>
  ResponseDispatcher &on(const TypedMessage<T> &message, F &&handler) {
    return on(message,
              std::function<void(const T &)>(std::forward<F>(handler)));
  }

  template <typename T, typename F>
  ResponseDispatcher &on(TypedMessage<T> &&, F &&) = delete;

  template <typename T>
  ResponseDispatcher &on(const TypedMessage<T> &message,
                         std::function<void(const T &)> handler) {
    const schema::Schema<T> *boundSchema = &message.schema();
    on(message.opcode(),
       [boundSchema, captured = std::move(handler)](IMessage &incoming) {
         auto &asBase = static_cast<AMessage &>(incoming);
         T value{};
         auto error = boundSchema->parseInto(asBase.args(), value);
         if (error) {
           return;
         }
         captured(value);
       });
    return *this;
  }

  /**
   * @brief Register a raw handler for an RPCMessage (routes by opcode).
   */
  ResponseDispatcher &on(const RPCMessage &message, Handler handler);

  /**
   * @brief Register a raw handler for an opcode string (no validation).
   */
  ResponseDispatcher &on(const std::string &opcode, Handler handler);

  /**
   * @brief Register a fallback handler for unrecognised opcodes.
   */
  ResponseDispatcher &onUnknown(Handler handler);

  /**
   * @brief Parse a raw server line and dispatch it.
   */
  void operator()(const std::string &line);

  /**
   * @brief Dispatch a pre-built IMessage directly.
   */
  void dispatch(IMessage &message);

 private:
  std::unordered_map<std::string, Handler> handlers_;
  Handler unknownHandler_;
};

}  // namespace zappy::rpc
