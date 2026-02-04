//
// Created by ahmed on 2025-12-29.
//

#ifndef FOOTBALL_SERVER_SNAPSHOT_H
#define FOOTBALL_SERVER_SNAPSHOT_H
#include <vector>

#include <football/GameEvent.h>

struct PlayerState {
  int id;
  float x;
  float y;
  float stamina;
  float health;
  Tick lastProcessedInputTick;
};

struct MatchSnapshot {
  int matchId;
  Tick tick;
  std::vector<PlayerState> players;
};

struct OutBoundSnapshot {
  int matchId;
  Tick tick;
  std::string json;
};

#endif // FOOTBALL_SERVER_SNAPSHOT_H
