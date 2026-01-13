//
// Created by ahmed on 2025-12-28.
//

#ifndef FOOTBALL_SERVER_PLAYER_H
#define FOOTBALL_SERVER_PLAYER_H
#include <string>

struct Player {
  int id;
  std::string name;
  float x{0.0f};
  float y{0.0f};

  float velocityX{0.0f};
  float velocityY{0.0f};

  float stamina{100.0f};
  float health{100.0f};
};

#endif // FOOTBALL_SERVER_PLAYER_H
