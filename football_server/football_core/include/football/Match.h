//
// Created by ahmed on 2025-12-28.
//

#ifndef FOOTBALL_SERVER_MATCH_H
#define FOOTBALL_SERVER_MATCH_H
#include <football/GameEvent.h>
#include <football/Player.h>
#include <football/Snapshot.h>
#include <queue>
#include <unordered_map>
#include <vector>

enum class InputAcceptResult {
  Accepted,
  DroppedTooOld,
  DroppedTooEarly,
  Deduped,
  Rejected
};

struct BufferedCmd {
  uint32_t tick;
  float ax, ay;
  uint16_t buttons;
};

struct PlayerInputState {
  std::deque<BufferedCmd> pending;           // sorted by tick ascending
  BufferedCmd currentIntent{0, 0.f, 0.f, 0}; // latest <= currentTick
  uint32_t lastProcessedTick = 0;
};

struct InputCmd {
  uint32_t matchId = 0;
  uint32_t clientTick = 0;
  float ax = 0.f; // normalized
  float ay = 0.f;
  uint16_t buttons = 0;
};

class Match {
public:
  static constexpr Tick MAX_FUTURE = 5;
  static constexpr Tick MAX_PAST = 5;
  static constexpr size_t MAX_PLAYER_BUFFER = 64;

  explicit Match(int matchId);

  template <typename... Args>
    requires std::constructible_from<GameEvent, Args...>
  void enqueueEvent(Args &&...args);

  InputAcceptResult enqueueInput(uint32_t playerId, const InputCmd &in);

  MatchSnapshot makeSnapshot() const;
  void processScheduledEventsForTick_();
  void processInputEvents();

  void applyMovementForPlayer_(uint32_t, const BufferedCmd &);
  void update();

  [[nodiscard]] int getId() const { return matchId; }

  void addPlayer(uint32_t uint32);

  [[nodiscard]] Tick getCurrentTick() const { return currentTick; }

  void removePlayer(uint32_t pid);

  [[nodiscard]] const std::vector<Player> &getPlayers() const {
    return players;
  }

#ifdef FOOTBALL_TESTING
  void set_current_tick(const Tick current_tick) { currentTick = current_tick; }
#endif

  PlayerInputState *getPlayerState(const uint32_t playerId);

private:
  int matchId;
  Tick currentTick{0};

  uint64_t nextSeq{0};

  std::vector<Player> players;
  std::priority_queue<GameEvent, std::vector<GameEvent>, GameEventLess>
      game_events;
  std::unordered_map<uint32_t, PlayerInputState> inputsByPlayer_;

  // Tunables (we'll externalize later)
  float maxSpeed; // units/sec
  float pitchMinX, pitchMaxX;
  float pitchMinY, pitchMaxY;
  float dtSeconds; // seconds per tick

  void processEvent(const GameEvent &event);

  void simulateMovement();

  void decayStamina();

  Player *findPlayer(uint32_t playerId);

  static float clamp(float v, float lo, float hi);
};

#include "Match.tpp"

#endif // FOOTBALL_SERVER_MATCH_H
