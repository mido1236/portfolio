//
// Created by ahmed on 2026-01-21.
//

#ifndef FOOTBALL_SERVER_LIB_INBOUNDQUEUE_H
#define FOOTBALL_SERVER_LIB_INBOUNDQUEUE_H
#include <football_server/Messages.h>
#include <moodycamel/concurrentqueue.h>

class InBoundQueue {
public:
  struct Config {
    uint32_t capacity;
    uint32_t reserved_for_disconnect;

    static Config Default() {
      return {.capacity = 10'000, .reserved_for_disconnect = 256};
    }
  };

  InBoundQueue() : InBoundQueue(Config::Default()) {}

  explicit InBoundQueue(const Config cfg)
      : capacity(cfg.capacity), reservedForDisconnect(std::min(
                                    cfg.reserved_for_disconnect, cfg.capacity)),
        permits_(capacity - reservedForDisconnect),
        disconnect_permits(reservedForDisconnect), q_(cfg.capacity) {}

  bool push_disconnect(InBoundMsg m) {
    if (!try_acquire(disconnect_permits))
      return false;
    if (!q_.enqueue(std::move(m))) {
      release(disconnect_permits);
      return false;
    }
    return true;
  }

  bool push(InBoundMsg m) {
    if (m.type == MsgType::Disconnect) {
      return push_disconnect(m);
    }
    if (!try_acquire(permits_))
      return false;

    if (!q_.enqueue(std::move(m))) {
      release(permits_);
      return false;
    }
    return true;
  }

  bool try_pop(InBoundMsg &out) {
    if (!q_.try_dequeue(out))
      return false;

    if (out.type == MsgType::Disconnect)
      release(disconnect_permits);
    else
      release(permits_);

    return true;
  }

  uint32_t in_flight_normal() const noexcept {
    // normal in-flight = (capacity - reserved) - normal_permits
    const int64_t p = permits_.load(std::memory_order_relaxed);
    return static_cast<uint32_t>((capacity - reservedForDisconnect) - p);
  }

  uint32_t in_flight_disconnect() const noexcept {
    const int64_t p = disconnect_permits.load(std::memory_order_relaxed);
    return static_cast<uint32_t>(reservedForDisconnect - p);
  }

  uint32_t approx_size() const noexcept {
    // exact in-flight total with this wrapper
    return in_flight_normal() + in_flight_disconnect();
  }

private:
  const uint32_t capacity;
  const uint32_t reservedForDisconnect;

  std::atomic<int64_t> permits_;
  std::atomic<int64_t> disconnect_permits;

  moodycamel::ConcurrentQueue<InBoundMsg> q_;

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

#endif // FOOTBALL_SERVER_LIB_INBOUNDQUEUE_H
