//
// Created by ahmed on 2026-01-25.
//

#ifndef FOOTBALL_SERVER_LIB_PUBLISHER_H
#define FOOTBALL_SERVER_LIB_PUBLISHER_H

#include <football_server/IPublisher.h>

struct CapturingPublisher final : IPublisher {
  void publishText(std::string topic, std::string text) override {
    lastTopic = std::move(topic);
    lastText = std::move(text);
    publishes++;
  }
  std::string lastTopic;
  std::string lastText;
  int publishes = 0;
};

#endif // FOOTBALL_SERVER_LIB_PUBLISHER_H
