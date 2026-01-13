//
// Created by ahmed on 2025-12-28.
//

#ifndef FOOTBALL_SERVER_GAMEEVENT_H
#define FOOTBALL_SERVER_GAMEEVENT_H
#include <cstdint>

using Tick = uint64_t;

enum class EventType { Move, Pass, Shoot, Tackle };

struct GameEvent {
  Tick tick;
  int playerId;
  uint32_t seq;
  EventType type;
  float targetX;
  float targetY;

  GameEvent(const int t, const int p, const EventType et, const float x,
            const float y)
      : tick(t), playerId(p), seq(0), type(static_cast<EventType>(et)),
        targetX(x), targetY(y) {}
};

struct GameEventLess {
  bool operator()(const GameEvent &lhs, const GameEvent &rhs) const {
    return lhs.seq > rhs.seq;
  }
};

#endif // FOOTBALL_SERVER_GAMEEVENT_H
