//
// Created by ahmed on 2025-12-29.
//

#ifndef FOOTBALL_SERVER_SNAPSHOTJSON_H
#define FOOTBALL_SERVER_SNAPSHOTJSON_H

#include <football/Snapshot.h>
#include <nlohmann/json.hpp>

using nlohmann::json;

inline void to_json(json &j, const PlayerState &p) {
  j = json{
      {"id", p.id},         {"x", p.x},
      {"y", p.y},           {"stamina", p.stamina},
      {"health", p.health}, {"lastProcessedTick", p.lastProcessedInputTick}};
}

inline void to_json(json &j, const MatchSnapshot &m) {
  j = json{{"matchId", m.matchId}, {"tick", m.tick}, {"players", m.players}};
}

inline void to_json(json &j, const InputCmd &p) {
  j = json{{"ax", p.ax},
           {"ay", p.ay},
           {"clientTick", p.clientTick},
           {"buttons", p.buttons},
           {"matchId", p.matchId}};
}

#endif // FOOTBALL_SERVER_SNAPSHOTJSON_H
