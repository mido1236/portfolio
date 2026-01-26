#include "football_server/GameServer.h"
#include "football_server/InBoundQueue.h"
#include "football_server/UwsHub.h"

#include <football/Match.h> // adjust
#include <gtest/gtest.h>

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

TEST(InputIntegrity, RejectsMissingPlayers) {
  Match match(1);

  const uint32_t playerId = 42;
  match.addPlayer(playerId);

  const uint32_t serverTick = 100;
  match.set_current_tick(serverTick);

  const auto res = match.enqueueInput(
      playerId, {static_cast<uint32_t>(match.getId()), serverTick, 0.5f});

  EXPECT_EQ(res, InputAcceptResult::Accepted);
  EXPECT_EQ(match.getPlayerState(playerId)->pending.back().ax, 0.5f);

  match.removePlayer(playerId);
  const auto res3 = match.enqueueInput(
      playerId, {static_cast<uint32_t>(match.getId()), serverTick, 0.5f});

  EXPECT_EQ(res3, InputAcceptResult::Rejected);
}

TEST(Input, BinaryIgnoredWhenNotInMatch) {
  InBoundQueue queue;
  UwsHub hub(queue);
  GameServer server(queue, hub);

  constexpr uint32_t playerId = 7;

  // Send binary input before JOIN
  std::vector<uint8_t> bogusInput = {0x01, 0x02, 0x03};
  queue.push(InBoundMsg::makeBinary(playerId,
                                    bogusInput.data(),
                                    bogusInput.size()));

  // Should not crash or enqueue anything
  EXPECT_NO_THROW(server.advance_tick());
}