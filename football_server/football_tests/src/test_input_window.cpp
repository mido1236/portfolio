#include <gtest/gtest.h>
#include <football/Match.h> // adjust

TEST(InputWindow, RejectsTooOldInput) {
  Match match(1);

  const uint32_t playerId = 42;
  match.addPlayer(playerId);

  const uint32_t serverTick = 100;
  match.set_current_tick(serverTick);

  const uint32_t tooOldTick = serverTick - (Match::MAX_PAST + 1);

  const auto res = match.enqueueInput(
      playerId, {static_cast<uint32_t>(match.getId()), tooOldTick});

  EXPECT_EQ(res, InputAcceptResult::DroppedTooOld);
}

TEST(InputWindow, RejectsTooEarlyInput) {
  Match match(1);

  const uint32_t playerId = 42;
  match.addPlayer(playerId);

  const uint32_t serverTick = 100;
  match.set_current_tick(serverTick);

  const uint32_t tooOldTick = serverTick + (Match::MAX_FUTURE + 1);

  const auto res = match.enqueueInput(
      playerId, {static_cast<uint32_t>(match.getId()), tooOldTick});

  EXPECT_EQ(res, InputAcceptResult::DroppedTooEarly);
}

TEST(InputWindow, AcceptsBoundaryTicks) {
  Match match(1);

  const uint32_t playerId = 42;
  match.addPlayer(playerId);

  const uint32_t serverTick = 100;
  match.set_current_tick(serverTick);

  const uint32_t oldestAllowed = serverTick - Match::MAX_PAST;
  const uint32_t newestAllowed = serverTick + Match::MAX_FUTURE;

  EXPECT_EQ(match.enqueueInput(playerId, {static_cast<uint32_t>(match.getId()),
                                          oldestAllowed}),
            InputAcceptResult::Accepted);
  EXPECT_EQ(match.enqueueInput(playerId, {static_cast<uint32_t>(match.getId()),
                                          newestAllowed}),
            InputAcceptResult::Accepted);
}

TEST(InputIntegrity, DetectsDuplicateInputs) {
  Match match(1);

  const uint32_t playerId = 42;
  match.addPlayer(playerId);

  const uint32_t serverTick = 100;
  match.set_current_tick(serverTick);

  const auto res = match.enqueueInput(
      playerId, {static_cast<uint32_t>(match.getId()), serverTick});

  EXPECT_EQ(res, InputAcceptResult::Accepted);

  EXPECT_EQ(match.getPlayerState(playerId)->pending.back().ax, 0.f);

  const auto res2 = match.enqueueInput(
      playerId, {static_cast<uint32_t>(match.getId()), serverTick, 0.5f});

  EXPECT_EQ(res2, InputAcceptResult::Deduped);

  EXPECT_EQ(match.getPlayerState(playerId)->pending.back().ax, 0.5f);
}