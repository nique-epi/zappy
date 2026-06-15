/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** AiTypes — shared RPC type aliases for the AI command handlers
*/

#pragma once

#include "Net/ClientContext.hpp"
#include "Rpc/Server/RPCServer.hpp"
#include "Rpc/Session/Session.hpp"

namespace zappy::server {

/// An AI client session: an RPC session carrying the per-connection context.
using AiSession = zappy::rpc::Session<ClientContext>;

/// The RPC server hosting the AI clients.
using AiServer = zappy::rpc::RPCServer<ClientContext>;

}  // namespace zappy::server
