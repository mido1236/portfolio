#include "football_server/InBoundQueue.h"
#include "football_server/UwsHub.h"

#include <football/Match.h> // adjust
#include <gtest/gtest.h>

TEST(Ingress, RespectsQueueSizeLimits) {
  constexpr uint32_t capacity = 5;
  constexpr uint32_t r = 2;
  InBoundQueue q({.capacity = capacity, .reserved_for_disconnect = r});

  for (int i = 0; i < capacity - r; i++)
    EXPECT_TRUE(q.push(InBoundMsg{.type = MsgType::Text}));
  EXPECT_EQ(q.approx_size(), capacity - r);
  for (int i = 0; i < 20; i++)
    EXPECT_FALSE(q.push(InBoundMsg{.type = MsgType::Text}));
  EXPECT_EQ(q.approx_size(), capacity - r);

  for (int i = 0; i < r; i++)
    EXPECT_TRUE(q.push(InBoundMsg{.type = MsgType::Disconnect}));
  EXPECT_EQ(q.approx_size(), capacity);
  for (int i = 0; i < 20; i++)
    EXPECT_FALSE(q.push(InBoundMsg{.type = MsgType::Disconnect}));
  EXPECT_EQ(q.approx_size(), capacity);
}

TEST(Ingress, RespectsTextMessageSizeLimits) {
  constexpr uint32_t capacity = 5;
  constexpr uint32_t r = 2;
  InBoundQueue q({.capacity = capacity, .reserved_for_disconnect = r});
  UwsHub::IngressStats stats;
  const UwsHub hub(q, {.maxTextBytes = 10, .maxBinaryBytes = 10}, stats);

  hub.onMessageIngress(42, "", uWS::OpCode::TEXT);
  EXPECT_EQ(stats.drop_text_too_large.load(std::memory_order::relaxed), 0);
  EXPECT_EQ(q.approx_size(), 1);

  hub.onMessageIngress(42, string(10, 'a'), uWS::OpCode::TEXT);
  EXPECT_EQ(stats.drop_text_too_large.load(std::memory_order::relaxed), 0);
  EXPECT_EQ(q.approx_size(), 2);

  hub.onMessageIngress(42, string(11, 'a'), uWS::OpCode::TEXT);
  EXPECT_EQ(stats.drop_text_too_large.load(std::memory_order::relaxed), 1);
  EXPECT_EQ(q.approx_size(), 2);
}

TEST(Ingress, RespectsBinaryMessageSizeLimits) {
  constexpr uint32_t capacity = 5;
  constexpr uint32_t r = 2;
  InBoundQueue q({.capacity = capacity, .reserved_for_disconnect = r});
  UwsHub::IngressStats stats;
  const UwsHub hub(q, {.maxTextBytes = 10, .maxBinaryBytes = 10}, stats);

  hub.onMessageIngress(42, "", uWS::OpCode::BINARY);
  EXPECT_EQ(stats.drop_bin_too_large.load(std::memory_order::relaxed), 0);
  EXPECT_EQ(q.approx_size(), 1);

  hub.onMessageIngress(42, string(10, 'a'), uWS::OpCode::BINARY);
  EXPECT_EQ(stats.drop_bin_too_large.load(std::memory_order::relaxed), 0);
  EXPECT_EQ(q.approx_size(), 2);

  hub.onMessageIngress(42, string(11, 'a'), uWS::OpCode::BINARY);
  EXPECT_EQ(stats.drop_bin_too_large.load(std::memory_order::relaxed), 1);
  EXPECT_EQ(q.approx_size(), 2);
}