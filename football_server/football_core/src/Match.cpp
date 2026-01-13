//
// Created by ahmed on 2025-12-28.
//

#include <cmath>
#include <football/Match.h>

Match::Match(const int matchId)
    : matchId(matchId), currentTick(0), maxSpeed(8.0f), pitchMinX(0.0f),
      pitchMaxX(105.0f), pitchMinY(0.0f), pitchMaxY(68.0f),
      dtSeconds(1.0f / 20.0f) {
  players.emplace_back(1, "A1", 10.0f, 10.0f, 0.0f, 0.0f, 100.0f, 100.0f);
}

Player *Match::findPlayer(const uint32_t playerId) {
  for (auto &player : players) {
    if (player.id == playerId)
      return &player;
  }
  return nullptr;
}

float Match::clamp(const float v, const float lo, const float hi) {
  return std::max(lo, std::min(hi, v));
}

void Match::processEvent(const GameEvent &event) {
  Player *player = findPlayer(event.playerId);

  if (!player)
    return;

  switch (event.type) {
  case EventType::Move: {
    const float dx = event.targetX - player->x;
    const float dy = event.targetY - player->y;
    const float len = std::sqrt(dx * dx + dy * dy);

    if (len < 0.001f) {
      player->velocityX = 0.0f;
      player->velocityY = 0.0f;
      break;
    }

    const float ux = dx / len;
    const float uy = dy / len;

    const float staminaFactor = clamp(player->stamina / 100.0f, 0.25f, 1.0f);
    const float speed = maxSpeed * staminaFactor;

    player->velocityX = ux * speed;
    player->velocityY = uy * speed;
    break;
  }
  case EventType::Pass:
  case EventType::Shoot:
  case EventType::Tackle:
    break;
  }
}

void Match::simulateMovement() {
  for (auto &p : players) {
    p.x += p.velocityX * dtSeconds;
    p.y += p.velocityY * dtSeconds;

    p.x = clamp(p.x, pitchMinX, pitchMaxX);
    p.y = clamp(p.y, pitchMinY, pitchMaxY);

    // Handle out of bounds
    if (p.x == pitchMinX || p.x == pitchMaxX)
      p.velocityX = 0;
    if (p.y == pitchMinY || p.y == pitchMaxY)
      p.velocityY = 0;
  }
}

void Match::decayStamina() {
  for (auto &p : players) {
    const float speed =
        std::sqrt(p.velocityX * p.velocityX + p.velocityY * p.velocityY);

    if (speed > 0.1f)
      p.stamina -= 0.35f;
    else
      p.stamina += 0.1f;

    p.stamina = clamp(p.stamina, 0.0f, 100.0f);

    if (p.stamina <= 5.0f) {
      p.velocityX *= 0.5f;
      p.velocityY *= 0.5f;
    }
  }
}

InputAcceptResult Match::enqueueInput(const uint32_t playerId,
                                      const InputCmd &in) {
  if (in.clientTick + MAX_PAST < currentTick)
    return InputAcceptResult::DroppedTooOld;
  if (in.clientTick > currentTick + MAX_FUTURE)
    return InputAcceptResult::DroppedTooEarly;

  // Clamp axes for sanity
  auto clamp = [](const float v) {
    if (v < -1.f)
      return -1.f;
    if (v > 1.f)
      return 1.f;
    return v;
  };

  const BufferedCmd cmd = {in.clientTick, clamp(in.ax), clamp(in.ay),
                           in.buttons};

  auto &st = inputsByPlayer_[playerId];
  auto &dq = st.pending;
  auto result = InputAcceptResult::Accepted;

  if (dq.empty() || dq.back().tick <= cmd.tick) {
    if (!dq.empty() && dq.back().tick == cmd.tick)
      dq.back() = cmd, result = InputAcceptResult::Deduped;
    else
      dq.push_back(cmd);
  } else {
    const auto it = std::ranges::lower_bound(
        dq, cmd, [](const BufferedCmd &a, const BufferedCmd &b) {
          return a.tick < b.tick;
        });
    if (it != dq.end() && it->tick == cmd.tick)
      *it = cmd, result = InputAcceptResult::Deduped;
    else
      dq.insert(it, cmd);
  }

  while (!dq.empty() && dq.front().tick + MAX_PAST < currentTick)
    dq.pop_front();

  while (dq.size() > MAX_PLAYER_BUFFER) {
    dq.pop_front();
  }

  return result;
}

MatchSnapshot Match::makeSnapshot() const {
  MatchSnapshot snap;

  snap.matchId = matchId;
  snap.tick = currentTick;
  snap.players.reserve(players.size());

  for (const auto &p : players) {
    const auto it = inputsByPlayer_.find(p.id);
    const auto last_tick =
        it != inputsByPlayer_.end() ? it->second.lastProcessedTick : 0;

    snap.players.push_back(
        PlayerState({p.id, p.x, p.y, p.stamina, p.health, last_tick}));
  }

  return snap;
}

void Match::processScheduledEventsForTick_() {
  while (!game_events.empty() && game_events.top().tick <= currentTick) {
    processEvent(game_events.top());
    game_events.pop();
  }
}
void Match::processInputEvents() {
  for (auto &[playerId, st] : inputsByPlayer_) {
    auto &pending = st.pending;

    while (!pending.empty() && pending.front().tick + MAX_PAST <= currentTick)
      pending.pop_front();

    while (!pending.empty() && pending.front().tick <= currentTick) {
      st.currentIntent = pending.front();
      st.lastProcessedTick = pending.front().tick;
      pending.pop_front();
    }

    applyMovementForPlayer_(playerId, st.currentIntent);
  }
}
void Match::applyMovementForPlayer_(const uint32_t playerId,
                                    const BufferedCmd &intent) {
  Player *p = findPlayer(playerId);
  if (!p)
    return;

  // Normalize input so diagonal isn't faster
  float ax = intent.ax;
  float ay = intent.ay;
  const float len = std::sqrt(ax * ax + ay * ay);
  if (len > 1.0f) {
    ax /= len;
    ay /= len;
  }

  const float staminaFactor = clamp(p->stamina / 100.0f, 0.25f, 1.0f);
  const float speed = maxSpeed * staminaFactor;

  p->velocityX = ax * speed;
  p->velocityY = ay * speed;
}

void Match::update() {
  processScheduledEventsForTick_();

  processInputEvents();

  simulateMovement();
  decayStamina();
  currentTick++;
}

void Match::addPlayer(uint32_t playerId) {
  players.push_back(Player(playerId, "P" + std::to_string(playerId), 0.0f, 0.0f,
                           0.0f, 0.0f, 100.0f, 100.0f));
}
