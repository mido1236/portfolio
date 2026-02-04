//
// Created by ahmed on 2026-02-03.
//

#ifndef FOOTBALL_SERVER_LIB_EGRESSSTATS_H
#define FOOTBALL_SERVER_LIB_EGRESSSTATS_H

#pragma once
#include <atomic>
#include <cstdint>

struct EgressStats {
  std::atomic<uint64_t> drop_queue_full{0};
  std::atomic<uint64_t> published{0};      // successfully enqueued (or written, your choice)
  std::atomic<uint64_t> redis_write_ok{0};
  std::atomic<uint64_t> redis_write_fail{0};
};

#endif // FOOTBALL_SERVER_LIB_EGRESSSTATS_H
