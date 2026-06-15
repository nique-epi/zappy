/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** AiActionPipeline — wires per-AI request buffering and action blocking
*/

#pragma once

#include "App/Scheduler/Scheduler.hpp"
#include "App/TimeUnit.hpp"
#include "Net/ClientContext.hpp"
#include "Rpc/Server/RPCServer.hpp"

namespace zappy::server {

/**
 * @brief Install the per-AI command-queue and action-blocking pipeline on
 *        @p server.
 *
 * After this call, every authenticated line is routed as follows:
 * - GUI sessions dispatch immediately (GUI commands carry no action cost).
 * - AI sessions push the line into their @ref ActionQueue. When the queue
 *   is already full (more than @ref maxPendingActions pending) the new line
 *   is silently dropped; otherwise, if the player has no action in flight,
 *   the front of the queue is scheduled for dispatch at `now + cost / f`,
 *   where the cost comes from @ref costOf and @p frequency is the reciprocal
 *   time unit. When the scheduled callback fires, the line is dispatched
 *   through @ref RPCServer::dispatchLine and the next pending line (if any)
 *   is immediately scheduled.
 *
 * Each AI player is blocked only by its own action: a long-running command
 * for one session never delays another.
 *
 * On disconnect, the queue and the in-flight flag are cleared; pending
 * scheduled callbacks become no-ops because they look the session up by fd.
 *
 * @param[in] server    The RPC server hosting the AI clients. Its lifetime
 *                      must outlive every scheduled callback.
 * @param[in] scheduler The scheduler used to defer dispatch by action cost.
 *                      Its lifetime must also outlive every callback.
 * @param[in] timeUnit Live reciprocal time unit `f` read at scheduling time,
 *                     so a GUI `sst` mid-game applies to subsequent actions.
 *                     Must outlive every scheduled callback.
 */
void installAiActionPipeline(zappy::rpc::RPCServer<ClientContext>& server,
                             Scheduler& scheduler, const TimeUnit& timeUnit);

}  // namespace zappy::server
