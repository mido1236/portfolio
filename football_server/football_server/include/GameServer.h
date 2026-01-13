//
// Created by ahmed on 2025-12-28.
//

#ifndef FOOTBALL_SERVER_GAMESERVER_H
#define FOOTBALL_SERVER_GAMESERVER_H
#include <concurrentqueue/moodycamel/concurrentqueue.h>
#include <memory>
#include <unordered_map>

#include <football/Match.h>
#include "UwsHub.h"

struct PendingInput {
  uint32_t playerId{0};
  InputCmd cmd;
};

struct JoinRequest {
  uint32_t playerId{0};
  uint32_t matchId{0};
};

enum class MsgType : uint8_t { Text, Binary };

struct InBoundMsg {
  uint32_t playerId{0};
  MsgType type;
  std::string_view text;
  std::span<const uint8_t> bytes;
};

class GameServer {
public:
  GameServer();

  void start();

private:
  bool running;
  int tickRate;

  moodycamel::ConcurrentQueue<PendingInput> inputQ_;
  moodycamel::ConcurrentQueue<JoinRequest> joinQ;
  moodycamel::ConcurrentQueue<InBoundMsg> inboundQ;
  std::unordered_map<uint32_t, std::unique_ptr<Match>> matches;
  UwsHub hub;

  void seedDemo();
  vector<InBoundMsg> drainInboundMessages();

  void gameLoop(int);

  void printSnapshotEvery(const Match &match, int everyNTicks);

  void handleBinaryMessage(uint32_t playerId, std::span<const uint8_t> bytes);
  void handleTextMessage(uint32_t playerId, std::string_view msg);
  void onMessage(uint32_t playerId, std::string_view msg);
  void onBinaryMessage(uint32_t playerId, std::span<const uint8_t> bytes);

  void joinMatch(uint32_t playerId, std::string_view msg);
};

#endif // FOOTBALL_SERVER_GAMESERVER_H
