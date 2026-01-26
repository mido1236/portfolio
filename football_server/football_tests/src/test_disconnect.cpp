#include "../../football_server_lib/include/football_server/GameServer.h"

#include <football/Match.h> // adjust
#include <gtest/gtest.h>

TEST(Disconnect, PlayerDisconnectsGracefully) {
  InBoundQueue queue;
  UwsHub hub(queue);
  GameServer server(queue, hub);

  server.addMatch(1);
  auto match = server.getMatch(1)->get();
  constexpr uint32_t playerId = 42;

  queue.push({playerId, MsgType::Text, "JOIN 1"});
  server.advance_tick();

  queue.push({playerId, MsgType::Disconnect});
  server.advance_tick();

  match = server.getMatch(1)->get();
  auto players = match->getPlayers();
  const auto it = std::ranges::find_if(
      players, [](const Player &p) { return p.id == playerId; });

  EXPECT_EQ(it, players.end());
}

TEST(Disconnect, PlayerLeavesMatchButStaysConnected) {
  InBoundQueue queue;
  UwsHub hub(queue);
  GameServer server(queue, hub);

  server.addMatch(1);
  auto match = server.getMatch(1)->get();
  constexpr uint32_t playerId = 42;

  queue.push({playerId, MsgType::Text, "JOIN 1"});
  server.advance_tick();

  auto players = match->getPlayers();
  auto it = std::ranges::find_if(
      players, [](const Player &p) { return p.id == playerId; });
  EXPECT_NE(it, players.end());

  queue.push({playerId, MsgType::Text, "LEAVE"});
  server.advance_tick();

  match = server.getMatch(1)->get();
  players = match->getPlayers();
  it = std::ranges::find_if(players,
                            [](const Player &p) { return p.id == playerId; });

  EXPECT_EQ(it, players.end());
}