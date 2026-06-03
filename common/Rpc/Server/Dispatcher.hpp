/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** Dispatcher<Ctx> — routes parsed protocol lines to registered handlers
*/

#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <utility>
#include "Rpc/Message/IMessage.hpp"
#include "Rpc/Message/MessageParser.hpp"
#include "Rpc/Message/TypedMessage.hpp"
#include "Rpc/Session/Session.hpp"
#include "Schema/Schema.hpp"

namespace zappy::rpc {

/**
 * @brief Routes incoming protocol lines to per-message handler functions.
 *
 * Two registration styles: typed (Schema validates and populates T) or raw
 * (handler receives the IMessage directly). On schema validation failure the
 * dispatcher invokes the invalid-message handler (replies "sbp" by default)
 * and skips the typed handler. Override it with onInvalid() when a protocol
 * uses a different rejection token (e.g. the AI protocol replies "ko").
 */
template <typename Ctx>
class Dispatcher {
 public:
  using Handler = std::function<void(Session<Ctx> &, IMessage &)>;

  /**
   * @brief Register a typed handler bound to a TypedMessage<T>.
   */
  template <typename T, typename F>
  Dispatcher &on(const TypedMessage<T> &message, F &&handler) {
    return on(message, std::function<void(Session<Ctx> &, const T &)>(
                           std::forward<F>(handler)));
  }

  template <typename T, typename F>
  Dispatcher &on(TypedMessage<T> &&, F &&) = delete;

  template <typename T>
  Dispatcher &on(const TypedMessage<T> &message,
                 std::function<void(Session<Ctx> &, const T &)> handler) {
    auto boundSchema = message.schemaPtr();
    on(message.opcode(), [this, boundSchema, captured = std::move(handler)](
                             Session<Ctx> &session, IMessage &incoming) {
      auto *asBase = dynamic_cast<AMessage *>(&incoming);
      if (asBase == nullptr) {
        return;
      }
      T value{};
      auto error = boundSchema->parseInto(asBase->args(), value);
      if (error) {
        if (invalidHandler_) {
          invalidHandler_(session, incoming);
        } else {
          session.send("sbp");
        }
        return;
      }
      captured(session, value);
    });
    return *this;
  }

  /**
   * @brief Register a raw handler for an opcode string (no validation).
   */
  Dispatcher &on(const std::string &opcode, Handler handler) {
    handlers_.emplace(opcode, std::move(handler));
    return *this;
  }

  /**
   * @brief Register a fallback handler for unknown opcodes.
   */
  Dispatcher &onUnknown(Handler handler) {
    unknownHandler_ = std::move(handler);
    return *this;
  }

  /**
   * @brief Register the handler invoked when schema validation fails.
   *
   * Receives the offending message so the reply can depend on the session
   * (e.g. "sbp" for a GUI client, "ko" for an AI client). When unset, the
   * dispatcher replies "sbp".
   */
  Dispatcher &onInvalid(Handler handler) {
    invalidHandler_ = std::move(handler);
    return *this;
  }

  /**
   * @brief Dispatch a pre-built IMessage directly.
   */
  void dispatch(Session<Ctx> &session, IMessage &message) {
    auto it = handlers_.find(message.opcode());
    if (it != handlers_.end()) {
      it->second(session, message);
    } else if (unknownHandler_) {
      unknownHandler_(session, message);
    }
  }

  /**
   * @brief Parse a raw protocol line and dispatch it.
   */
  void operator()(Session<Ctx> &session, const std::string &line) {
    ParsedMessage message = parseMessage(line);
    if (message.opcode().empty()) {
      return;
    }
    dispatch(session, message);
  }

 private:
  std::unordered_map<std::string, Handler> handlers_;
  Handler unknownHandler_;
  Handler invalidHandler_;
};

}  // namespace zappy::rpc
