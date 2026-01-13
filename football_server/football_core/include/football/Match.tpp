//
// Created by ahmed on 2025-12-29.
//

#ifndef FOOTBALL_SERVER_MATCH_TPP
#define FOOTBALL_SERVER_MATCH_TPP
#include <concepts>

template <typename... Args>
  requires std::constructible_from<GameEvent, Args...>
void Match::enqueueEvent(Args &&...args) {
  auto event = GameEvent(std::forward<Args>(args)...);

  event.seq = nextSeq++;

  if (event.tick > currentTick + MAX_FUTURE ||
      event.tick + MAX_PAST < currentTick)
    return;
  game_events.push(event);
};

#endif // FOOTBALL_SERVER_MATCH_TPP
