#include "football_server/InBoundQueue.h"
#include "football_server/UwsHub.h"

#include <football/Match.h> // adjust
#include <gtest/gtest.h>

TEST(InputWindow, RespectsQueueSizeLimits) {
  constexpr uint32_t capacity = 5;
  constexpr uint32_t r = 2;
  InBoundQueue q({.capacity = capacity, .reserved_for_disconnect = r});

  for (int i = 0; i < capacity - r; i++)
    EXPECT_EQ(q.push({}), true);
  for (int i = 0; i < 20; i++)
    EXPECT_EQ(q.push({}), false);

  for (int i = 0; i < r; i++)
    EXPECT_EQ(q.push({.type = MsgType::Disconnect}), true);
  for (int i = 0; i < 20; i++)
    EXPECT_EQ(q.push({.type = MsgType::Disconnect}), false);
}

TEST(InputWindow, RespectsTextMessageSizeLimits) {
  constexpr uint32_t capacity = 5;
  constexpr uint32_t r = 2;
  InBoundQueue q({.capacity = capacity, .reserved_for_disconnect = r});
  UwsHub::IngressStats stats;
  const UwsHub hub(q, {.maxTextBytes = 10, .maxBinaryBytes = 10}, stats);

  hub.onMessageIngress(42, "", uWS::OpCode::TEXT);
  EXPECT_EQ(stats.drop_text_too_large, 0);

  hub.onMessageIngress(42, string(10, 'a'), uWS::OpCode::TEXT);
  EXPECT_EQ(stats.drop_text_too_large, 0);

  hub.onMessageIngress(42, string(11, 'a'), uWS::OpCode::TEXT);
  EXPECT_EQ(stats.drop_text_too_large, 1);
}

TEST(InputWindow, RespectsBinaryMessageSizeLimits) {
  constexpr uint32_t capacity = 5;
  constexpr uint32_t r = 2;
  InBoundQueue q({.capacity = capacity, .reserved_for_disconnect = r});
  UwsHub::IngressStats stats;
  const UwsHub hub(q, {.maxTextBytes = 10, .maxBinaryBytes = 10}, stats);

  hub.onMessageIngress(42, "", uWS::OpCode::BINARY);
  EXPECT_EQ(stats.drop_bin_too_large, 0);

  hub.onMessageIngress(42, string(10, 'a'), uWS::OpCode::BINARY);
  EXPECT_EQ(stats.drop_bin_too_large, 0);

  hub.onMessageIngress(42, string(11, 'a'), uWS::OpCode::BINARY);
  EXPECT_EQ(stats.drop_bin_too_large, 1);
}