//
// Created by ahmed on 2026-01-25.
//

#ifndef FOOTBALL_SERVER_LIB_IPUBLISHER_H
#define FOOTBALL_SERVER_LIB_IPUBLISHER_H

#include <string>

struct IPublisher {
  virtual ~IPublisher() = default;
  virtual void publishText(std::string topic, std::string text) = 0;
};

#endif // FOOTBALL_SERVER_LIB_IPUBLISHER_H
