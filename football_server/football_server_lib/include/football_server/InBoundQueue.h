//
// Created by ahmed on 2026-01-21.
//

#ifndef FOOTBALL_SERVER_LIB_INBOUNDQUEUE_H
#define FOOTBALL_SERVER_LIB_INBOUNDQUEUE_H
#include <football_server/Messages.h>
#include <moodycamel/concurrentqueue.h>
#include <span>
#include <string_view>

class InBoundQueue {
public:
  void push(InBoundMsg m) {
    q_.enqueue(std::move(m));
  }
  bool try_pop(InBoundMsg &out) { return q_.try_dequeue(out); }

private:
  moodycamel::ConcurrentQueue<InBoundMsg> q_;
};

#endif // FOOTBALL_SERVER_LIB_INBOUNDQUEUE_H
