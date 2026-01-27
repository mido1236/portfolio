#include "football_server/InBoundQueue.h"

#include <football/Match.h> // adjust
#include <gtest/gtest.h>

TEST(InputWindow, RespectsQueueLimits) {
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
