/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** AiActionPipeline — wires per-AI request buffering and action blocking
*/

#include "Net/AiActionPipeline.hpp"
#include <optional>
#include <string>
#include "App/ActionCosts.hpp"
#include "App/Scheduler/ActionTiming.hpp"
#include "Rpc/Session/Session.hpp"

namespace zappy::server {

using AiSession = zappy::rpc::Session<ClientContext>;
using AiServer = zappy::rpc::RPCServer<ClientContext>;

namespace {

std::string firstToken(const std::string& line) {
  const auto end = line.find_first_of(" \t");
  if (end == std::string::npos) {
    return line;
  }
  return line.substr(0, end);
}

void scheduleNext(AiServer& server, Scheduler& scheduler, int frequency,
                  int fileDescriptor);

void dispatchAndAdvance(AiServer& server, Scheduler& scheduler, int frequency,
                        int fileDescriptor, const std::string& line) {
  AiSession* session = server.session(fileDescriptor);
  if (session == nullptr) {
    return;
  }
  server.dispatchLine(*session, line);
  session->ctx().actionInFlight = false;
  scheduleNext(server, scheduler, frequency, fileDescriptor);
}

void scheduleNext(AiServer& server, Scheduler& scheduler, int frequency,
                  int fileDescriptor) {
  AiSession* session = server.session(fileDescriptor);
  if (session == nullptr) {
    return;
  }
  std::optional<std::string> next = session->ctx().pendingActions.pop();
  if (!next.has_value()) {
    return;
  }
  session->ctx().actionInFlight = true;
  const int cost = costOf(firstToken(*next));
  const Scheduler::TimePoint deadline =
      Scheduler::Clock::now() + actionDuration(cost, frequency);
  scheduler.scheduleAt(deadline, [&server, &scheduler, frequency,
                                  fileDescriptor, line = *next]() {
    dispatchAndAdvance(server, scheduler, frequency, fileDescriptor, line);
  });
}

}  // namespace

void installAiActionPipeline(AiServer& server, Scheduler& scheduler,
                             int frequency) {
  server.onAuthenticatedLine([&server, &scheduler, frequency](
                                 AiSession& session, const std::string& line) {
    if (session.ctx().type != ClientType::Ai) {
      server.dispatchLine(session, line);
      return;
    }
    const std::size_t inFlight = session.ctx().actionInFlight ? 1U : 0U;
    if (session.ctx().pendingActions.size() + inFlight >= maxPendingActions) {
      return;
    }
    session.ctx().pendingActions.push(line);
    if (!session.ctx().actionInFlight) {
      scheduleNext(server, scheduler, frequency, session.fd());
    }
  });
}

}  // namespace zappy::server
