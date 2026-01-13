//
// Created by ahmed on 2025-12-28.
//

#include <GameServer.h>
#include <Parsing.h>
#include <Topics.h>
#include <chrono>
#include <football/SnapshotJson.h>
#include <iostream>
#include <memory>
#include <ranges>
#include <thread>

GameServer::GameServer() : running(false), tickRate(5) {}

void GameServer::seedDemo() {
  auto m = std::make_unique<Match>(1);

  // m->enqueueEvent(1, 1, EventType::Move, 52.5f, 34.0f);
  // m->enqueueEvent(15, 2, EventType::Move, 70.0f, 50.0f);
  // m->enqueueEvent(25, 3, EventType::Move, 40.0f, 20.0f);
  // m->enqueueEvent(45, 4, EventType::Move, 60.0f, 30.0f);

  matches.emplace(1, std::move(m));
}

vector<InBoundMsg> GameServer::drainInboundMessages() {
  InBoundMsg m;
  vector<InBoundMsg> batch;

  while (inboundQ.try_dequeue(m)) {
    batch.push_back(m);
  }
  return batch;
}

void GameServer::gameLoop(const int seconds) {
  using clock = std::chrono::steady_clock;
  const auto tickDur = std::chrono::milliseconds(1000 / tickRate);
  const auto end = clock::now() + std::chrono::seconds(seconds);

  while (running && clock::now() < end) {
    auto start = clock::now();

    for (const auto &[playerId, type, text, bytes] : drainInboundMessages()) {
      switch (type) {
      case MsgType::Text:
        handleTextMessage(playerId, text);
        break;
      case MsgType::Binary:
        handleBinaryMessage(playerId, bytes);
        break;
      }
    }

    for (const auto &m : matches | std::views::values) {
      m->update();
      json j = m->makeSnapshot();
      hub.publishText(matchTopic(m->getId()), j.dump(2));
    }

    if (auto elapsed = clock::now() - start; elapsed < tickDur)
      std::this_thread::sleep_for(tickDur - elapsed);
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

  if (!parseInputCmd(bytes, cmd))
    return;
  if (const auto it = matches.find(cmd.matchId); it != matches.end())
    it->second->enqueueInput(playerId, cmd);
}

void GameServer::handleTextMessage(const uint32_t playerId,
                                   const std::string_view msg) {
  if (msg.starts_with("JOIN ") && msg.size() > 5) {
    joinMatch(playerId, msg);
  }
}

void GameServer::onMessage(const uint32_t playerId,
                           const std::string_view msg) {
  inboundQ.enqueue(InBoundMsg{playerId, MsgType::Text, msg});
}

void GameServer::onBinaryMessage(const uint32_t playerId,
                                 const std::span<const uint8_t> bytes) {
  inboundQ.enqueue(InBoundMsg{playerId, MsgType::Binary, "", bytes});
}
void GameServer::joinMatch(const uint32_t playerId,
                           const std::string_view msg) {
  uint32_t matchId;
  try {
    matchId = std::stoul(string(msg.substr(5)));
    if (const auto it = matches.find(matchId); it != matches.end())
      it->second->addPlayer(playerId);
  } catch (...) {
    hub.joinMatch(playerId, -1);
    return;
  }
  hub.joinMatch(playerId, matchId);
}

void GameServer::start() {
  seedDemo();

  hub.setOnMessage(std::bind_front(&GameServer::onMessage, this));
  hub.setOnBinaryMessage(std::bind_front(&GameServer::onBinaryMessage, this));
  hub.start(9001);

  running = true;
  gameLoop(10);
  running = false;
}
