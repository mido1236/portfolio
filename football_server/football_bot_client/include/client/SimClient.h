//
// Created by ahmed on 2026-04-16.
//

#ifndef FOOTBALL_SERVER_LIB_SIMCLIENT_H
#define FOOTBALL_SERVER_LIB_SIMCLIENT_H
#include <cstdint>

struct PerSocketData {
  int playerId{-1};
  uint32_t matchId{0};
};

class SimClient {
  // ix::initNetSystem();

public:
  void connect();
  void run();
  void disconnect();

private:
  void sendInput();
};

#endif // FOOTBALL_SERVER_LIB_SIMCLIENT_H
