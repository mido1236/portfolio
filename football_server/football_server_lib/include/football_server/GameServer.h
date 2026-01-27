//
// Created by ahmed on 2025-12-28.
//

#ifndef FOOTBALL_SERVER_GAMESERVER_H
#define FOOTBALL_SERVER_GAMESERVER_H
#include "IPublisher.h"
#include "InBoundQueue.h"

#include <concurrentqueue/moodycamel/concurrentqueue.h>
#include <memory>
#include <unordered_map>

#include <football/Match.h>
#include <football_server/UwsHub.h>

#ifdef FOOTBALL_TESTING
#include <chrono>
#include <thread>
#endif

struct PendingInput {
  uint32_t playerId{0};
  InputCmd cmd;
};

struct JoinRequest {
  uint32_t playerId{0};
  uint32_t matchId{0};
};

struct TickAgg {
  u_int64 ticks = 0;
  uint64_t drainedMsgs = 0;
  uint64_t snapshotsPublished = 0;
  double simMsSum = 0;
  double simMsMax = 0;
  double sleptMs = 0;

  void reset() { *this = TickAgg{}; }
};

class GameServer {
public:
  GameServer(InBoundQueue &q, UwsHub &h, IPublisher &pub)
      : running(false), tickRate(1), inboundQ(q), hub(h), pub_(pub) {}

  void start();

#ifdef FOOTBALL_TESTING
  void addMatch(uint32_t matchId) {
    matches.emplace(matchId, std::make_unique<Match>(matchId));
  };

  void advance_tick() {
    using clock = std::chrono::steady_clock;
    process_game_tick(std::chrono::milliseconds(1000 / 20),
                      clock::now() + std::chrono::seconds(1));
  }

  std::unique_ptr<Match> *getMatch(const uint32_t matchId) {
    return !matches.contains(matchId) ? nullptr : &matches[matchId];
  };
#endif

private:
  bool running;
  int tickRate;
  InBoundQueue &inboundQ;
  IPublisher &pub_;
  UwsHub &hub;
  std::unordered_map<uint32_t, std::unique_ptr<Match>> matches;
  std::unordered_map<uint32_t, uint32_t> playerToMatch;

  void seedDemo();
  vector<InBoundMsg> drainInboundMessages() const;

  void handleDisconnect(uint32_t pid, DisconnectReason reason, int code,
                        const string &msg);
  void gameLoop(int);

  void process_game_tick(
      std::chrono::milliseconds tickDur,
      std::chrono::time_point<std::chrono::steady_clock> nextReport);

  void printSnapshotEvery(const Match &match, int everyNTicks);

  void handleBinaryMessage(uint32_t playerId, std::span<const uint8_t> bytes);

  void handleTextMessage(uint32_t playerId, std::string_view msg);

  void joinMatch(uint32_t playerId, std::string_view msg);

  void
  printMetrics(std::chrono::time_point<std::chrono::steady_clock> &nextReport,
               TickAgg agg);
};

#endif // FOOTBALL_SERVER_GAMESERVER_H
