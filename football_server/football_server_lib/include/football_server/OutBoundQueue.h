//
// Created by ahmed on 2026-01-21.
//

#ifndef FOOTBALL_SERVER_LIB_OUTBOUNDQUEUE_H
#define FOOTBALL_SERVER_LIB_OUTBOUNDQUEUE_H
#include <football/Snapshot.h>
#include <moodycamel/concurrentqueue.h>

class OutBoundQueue {
public:
  struct Config {
    uint32_t capacity;

    static Config Default() { return {.capacity = 10'000}; }
  };

  OutBoundQueue() : OutBoundQueue(Config::Default()) {}

  explicit OutBoundQueue(const Config cfg)
      : capacity(cfg.capacity), permits_(capacity), q_(cfg.capacity) {}

  bool push(MatchSnapshot m) {
    if (!try_acquire(permits_))
      return false;

    if (!q_.enqueue(std::move(m))) {
      release(permits_);
      return false;
    }
    return true;
  }

  bool try_pop(MatchSnapshot &out) {
    if (!q_.try_dequeue(out))
      return false;

    release(permits_);

    return true;
  }

  uint32_t in_flight_normal() const noexcept {
    // normal in-flight = (capacity - reserved) - normal_permits
    const int64_t p = permits_.load(std::memory_order_relaxed);
    return static_cast<uint32_t>((capacity)-p);
  }

  uint32_t approx_size() const noexcept {
    // exact in-flight total with this wrapper
    return in_flight_normal();
  }

private:
  const uint32_t capacity;

  std::atomic<int64_t> permits_;

  moodycamel::ConcurrentQueue<MatchSnapshot> q_;

  static bool try_acquire(std::atomic<int64_t> &permits) {
    int64_t cur = permits.load(std::memory_order_relaxed);
    while (cur > 0) {
      if (!permits.compare_exchange_weak(cur, cur - 1,
                                         std::memory_order_acquire,
                                         std::memory_order_relaxed))
        return true;
    }
    return false;
  }

  static void release(std::atomic<int64_t> &permits) noexcept {
    permits.fetch_add(1, std::memory_order_release);
  }
};

#endif // FOOTBALL_SERVER_LIB_OUTBOUNDQUEUE_H
