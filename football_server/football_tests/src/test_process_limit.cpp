#include "../include/publisher.h"
#include "football_server/GameServer.h"
#include "football_server/InBoundQueue.h"
#include "football_server/UwsHub.h"

#include <football/Match.h> // adjust
#include <gtest/gtest.h>

TEST(ProcessLimit, ProcessOnlyUpToLimit) {
  InBoundQueue queue({.capacity = 100, .reserved_for_disconnect = 10});
  UwsHub hub(queue);
  GameServer server(queue, hub, hub);

  constexpr uint32_t playerId = 7;

  ASSERT_TRUE(queue.push(InBoundMsg::makeBinary(playerId, nullptr, 0)));
  size_t res = server.processInbound(5);
  EXPECT_EQ(res, 1);
  EXPECT_EQ(queue.approx_size(), 0);

  res = server.processInbound(5);
  EXPECT_EQ(res, 0);
  EXPECT_EQ(queue.approx_size(), 0);

  for (int i = 0; i < 50; i++)
    ASSERT_TRUE(queue.push(InBoundMsg::makeBinary(playerId, nullptr, 0)));
  res = server.processInbound(5);

  EXPECT_EQ(res, 5);
  EXPECT_EQ(queue.approx_size(), 45);
}