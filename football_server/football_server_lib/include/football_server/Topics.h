//
// Created by ahmed on 2026-01-04.
//

#ifndef FOOTBALL_SERVER_TOPICS_H
#define FOOTBALL_SERVER_TOPICS_H
#include <string>

static std::string matchTopic(const uint32_t matchId)
{
    return "match." + std::to_string(matchId) + ".snapshots";
}

#endif //FOOTBALL_SERVER_TOPICS_H
