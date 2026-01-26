//
// Created by ahmed on 2025-12-28.
//

#include <chrono>
#include <football/SnapshotJson.h>
#include <football_server/GameServer.h>
#include <football_server/Parsing.h>
#include <football_server/Topics.h>
#include <iostream>
#include <memory>
#include <ranges>
#include <thread>

void GameServer::seedDemo() {
  auto m = std::make_unique<Match>(1);

  // m->pushEvent(1, 1, EventType::Move, 52.5f, 34.0f);
  // m->pushEvent(15, 2, EventType::Move, 70.0f, 50.0f);
  // m->pushEvent(25, 3, EventType::Move, 40.0f, 20.0f);
  // m->pushEvent(45, 4, EventType::Move, 60.0f, 30.0f);

  matches.emplace(1, std::move(m));
}

vector<InBoundMsg> GameServer::drainInboundMessages() {
  InBoundMsg m;
  vector<InBoundMsg> batch;

  while (inboundQ.try_pop(m)) {
    batch.push_back(m);
  }
  return batch;
}
void GameServer::handleDisconnect(uint32_t pid, DisconnectReason reason,
                                  int code, const string &msg) {
  if (const auto pit = playerToMatch.find(pid); pit != playerToMatch.end()) {
    if (const auto it = matches.find(pit->second); it != matches.end()) {
      it->second->removePlayer(pid);
    }
    playerToMatch.erase(pid);
  }
  std::cout << "Disconnected: " << msg << std::endl;
}

void GameServer::process_game_tick(
    const std::chrono::milliseconds tickDur,
    std::chrono::time_point<std::chrono::steady_clock> nextReport) {
  TickAgg agg;
  const auto start = std::chrono::steady_clock::now();

  const auto batch = drainInboundMessages();
  agg.drainedMsgs += batch.size();
  for (const auto &[playerId, type, text, bytes, reason, code, msg] : batch) {
    switch (type) {
    case MsgType::Text:
      handleTextMessage(playerId, text);
      break;
    case MsgType::Binary:
      handleBinaryMessage(playerId, bytes);
      break;
    case MsgType::Disconnect:
      handleDisconnect(playerId, reason, code, msg);
    default:;
    }
  }

  for (const auto &m : matches | std::views::values) {
    m->update();
    json j = m->makeSnapshot();
    hub.publishText(matchTopic(m->getId()), j.dump());
    agg.snapshotsPublished++;
  }

  const auto elapsed = std::chrono::steady_clock::now() - start;
  const double simMs =
      std::chrono::duration<double, std::milli>(elapsed).count();
  agg.simMsSum += simMs;
  agg.simMsMax = std::max(agg.simMsMax, simMs);

  double sleptMs = 0;
  if (elapsed < tickDur) {
    std::this_thread::sleep_for(tickDur - elapsed);
    sleptMs =
        std::chrono::duration<double, std::milli>(tickDur - elapsed).count();
  }

  agg.sleptMs += sleptMs;
  agg.ticks++;

  printMetrics(nextReport, agg);
}

void GameServer::gameLoop(const int seconds) {
  using clock = std::chrono::steady_clock;
  const auto tickDur = std::chrono::milliseconds(1000 / tickRate);
  const auto end = clock::now() + std::chrono::seconds(seconds);
  const auto nextReport = clock::now() + std::chrono::seconds(1);

  while (running && clock::now() < end) {
    process_game_tick(tickDur, nextReport);
  }
}

void GameServer::printSnapshotEvery(const Match &match, const int everyNTicks) {
  if (everyNTicks <= 0)
    return;
  if (match.getCurrentTick() % static_cast<Tick>(everyNTicks) != 0)
    return;

  auto snap = match.makeSnapshot();
  const json j = snap;

  std::cout << j.dump(2) << std::endl;
}

void GameServer::printMetrics(
    std::chrono::time_point<std::chrono::steady_clock> &nextReport,
    TickAgg agg) {
  if (const auto now = std::chrono::steady_clock::now(); now >= nextReport) {
    std::cout << "ticks: " << agg.ticks << ", msgs: " << agg.drainedMsgs
              << ", snapshots: " << agg.snapshotsPublished
              << ", simMsSum: " << agg.simMsSum
              << ", simMsMax: " << agg.simMsMax << ", sleptMs: " << agg.sleptMs
              << std::endl;
    agg.reset();
    nextReport = now + std::chrono::seconds(1);
  }
}

static EventType parseType(const std::string &s) {
  if (s == "Move")
    return EventType::Move;
  if (s == "Pass")
    return EventType::Pass;
  if (s == "Shoot")
    return EventType::Shoot;
  if (s == "Tackle")
    return EventType::Tackle;
  return EventType::Move;
}

void GameServer::handleBinaryMessage(const uint32_t playerId,
                                     const std::span<const uint8_t> bytes) {
  InputCmd cmd;

  if (!playerToMatch.contains(playerId) || !parseInputCmd(bytes, cmd))
    return;
  if (const auto it = matches.find(cmd.matchId); it != matches.end())
    it->second->enqueueInput(playerId, cmd);
}

void GameServer::handleTextMessage(const uint32_t playerId,
                                   const std::string_view msg) {
  if (msg.starts_with("JOIN ") && msg.size() > 5) {
    joinMatch(playerId, msg);
  }
  if (msg == "LEAVE") {
    // reuse your disconnect teardown path (but reason differs)
    handleDisconnect(playerId, DisconnectReason::ClosedByPeer, 0, "LEAVE");
  }
}

void GameServer::joinMatch(const uint32_t playerId,
                           const std::string_view msg) {
  uint32_t matchId;
  try {
    matchId = std::stoul(string(msg.substr(5)));
    if (const auto it = matches.find(matchId); it != matches.end()) {
      it->second->addPlayer(playerId);
      playerToMatch[playerId] = matchId;
    }
  } catch (...) {
    hub.joinMatch(playerId, -1);
    return;
  }
  hub.joinMatch(playerId, matchId);
}

void GameServer::start() {
  seedDemo();

  hub.start(9001);

  running = true;
  gameLoop(500);
  running = false;
}

#ifdef FOOTBALL_TESTING
void GameServer::addMatch(uint32_t matchId) {
  matches.emplace(matchId, std::make_unique<Match>(matchId));
}
#endif
