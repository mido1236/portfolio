//
// Created by ahmed on 2026-02-03.
//

#include <football/SnapshotJson.h>
#include <football_server/RedisWriter.h>
#include <iostream>
#include <sw/redis++/redis++.h>
#include <vector>

RedisWriter::RedisWriter(OutBoundQueue &q, EgressStats &stats, Config cfg)
    : q_(q), stats_(stats), cfg_(std::move(cfg)) {}

RedisWriter::~RedisWriter() { stop(); }

void RedisWriter::start() {
  if (running_.exchange(true))
    return;

  // Connect before thread starts so failures are immediate & obvious.
  sw::redis::ConnectionOptions opts;
  opts.host = cfg_.host;
  opts.port = cfg_.port;
  opts.password = cfg_.password;
  opts.tls.enabled = true;
  opts.tls.sni = opts.host;
  opts.tls.cacert = "cacert.pem";
  redis_ = std::make_unique<sw::redis::Redis>(opts);

  th_ = std::thread(&RedisWriter::run, this);
}

void RedisWriter::stop() {
  if (!running_.exchange(false))
    return;
  if (th_.joinable())
    th_.join();
  redis_.reset();
}

std::string RedisWriter::getSnapshotsStreamName(
    const std::vector<MatchSnapshot>::value_type &s) const {
  return "match:" + std::to_string(s.matchId) + ":" + cfg_.stream;
}

std::string RedisWriter::getLatestStreamName(
    std::vector<MatchSnapshot>::value_type &s) const {
  return "match:" + std::to_string(s.matchId) + ":latest";
}
void RedisWriter::run() const {
  using namespace std::chrono;

  std::vector<MatchSnapshot> batch;
  batch.reserve(cfg_.max_batch);

  while (running_.load(std::memory_order_relaxed)) {
    batch.clear();

    MatchSnapshot msg;
    while (batch.size() < cfg_.max_batch && q_.try_pop(msg)) {
      batch.push_back(std::move(msg));
    }

    if (batch.empty()) {
      std::this_thread::sleep_for(cfg_.idle_sleep);
      continue;
    }

    // Write each message (simple & robust).
    // You can pipeline later if you want to optimize.
    for (auto &s : batch) {
      try {
        // XADD snapshots * matchId <...> tick <...> json <...>
        ::json j = s;
        // std::cout << "Attempting to redis write:" << std::endl
        //           << j.dump() << std::endl;
        redis_->xadd<std::pair<std::string, std::string>>(
            getSnapshotsStreamName(s), "*",
            {{"matchId", std::to_string(s.matchId)},
             {"tick", std::to_string(s.tick)},
             {"json", j.dump()}});

        redis_->set(getLatestStreamName(s), j.dump());

        stats_.redis_write_ok.fetch_add(1, std::memory_order_relaxed);
      } catch (const std::exception &e) {
        stats_.redis_write_fail.fetch_add(1, std::memory_order_relaxed);
        std::cout << e.what() << std::endl;
        // Option: backoff or retry; for now just continue (portfolio-friendly
        // simplicity).
      }
    }

    if (cfg_.enable_trim) {
      try {
        // Approx trim to bound memory: XTRIM snapshots MAXLEN ~ trim_maxlen
        redis_->xtrim(cfg_.stream, cfg_.trim_maxlen, true);
      } catch (...) {
        // non-fatal
      }
    }
  }
}
