//
// Created by ahmed on 2026-02-03.
//

#ifndef FOOTBALL_SERVER_LIB_REDISWRITER_H
#define FOOTBALL_SERVER_LIB_REDISWRITER_H

#include <atomic>
#include <chrono>
#include <football_server/EgressStats.h>
#include <football_server/OutBoundQueue.h>
#include <string>
#include <thread>

namespace sw::redis {
class Redis;
}

class RedisWriter {
public:
  struct Config {
    std::string host = "";
    int port = 0;
    std::string password = "";
    std::string stream = "snapshots";
    bool enable_trim = true;
    long long trim_maxlen = 50000; // XTRIM MAXLEN ~
    size_t max_batch = 256;        // drain per loop
    std::chrono::milliseconds idle_sleep{2};
  };

  RedisWriter(OutBoundQueue &q, EgressStats &stats, Config cfg = Config());
  ~RedisWriter();

  void start();
  void stop();
  std::string getSnapshotsStreamName(const MatchSnapshot &s) const;
  static std::string getLatestStreamName(const MatchSnapshot &s);
  bool process_batch(std::vector<MatchSnapshot> &batch, bool sleep_if_empty);

private:
  void run();

  OutBoundQueue &q_;
  EgressStats &stats_;
  Config cfg_;

  std::atomic<bool> running_{false};
  std::thread th_;
  std::unique_ptr<sw::redis::Redis> redis_;
};

#endif // FOOTBALL_SERVER_LIB_REDISWRITER_H
