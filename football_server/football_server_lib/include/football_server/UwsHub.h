// Created by ahmed on 2025-12-30.
//

#ifndef FOOTBALL_SERVER_UWSHUB_H
#define FOOTBALL_SERVER_UWSHUB_H

#pragma once

#include "IPublisher.h"
#include "InBoundQueue.h"

#include <span>
#include <uwebsockets/App.h>

using std::atomic;
using std::function;
using std::mutex;
using std::string;
using std::string_view;
using std::thread;
using std::vector;

struct PerSocketData {
  int playerId{-1};
  uint32_t matchId{0};
};

class UwsHub final : public IPublisher {
public:
  struct IngressLimits {
    size_t maxTextBytes = 256;
    size_t maxBinaryBytes = 256;
  };

  struct IngressStats {
    std::atomic<uint64_t> drop_text_too_large{0};
    std::atomic<uint64_t> drop_bin_too_large{0};
    std::atomic<uint64_t> drop_queue_full{0};
    std::atomic<uint64_t> drop_unsupported_opcode{0};
  };

  using Ws = uWS::WebSocket<false, true, PerSocketData>;

  explicit UwsHub(InBoundQueue &q)
      : UwsHub(q, IngressLimits{}, defaultStats()) {}

  explicit UwsHub(InBoundQueue &q, const IngressLimits &limits,
                  IngressStats &stats)
      : inboundQ(q), limits(limits), stats(stats) {}

  ~UwsHub() override { stop(); };

  void start(int port);

  void stop();

  void broadcast(const std::string &text);

  void broadcastBinary(std::vector<uint8_t> bytes);

  void publishBinary(std::string topic, std::vector<uint8_t> bytes) const;

  void publishText(string topic, string text) override;

  void joinMatch(uint32_t playerId, uint32_t matchId);

  void onMessageIngress(uint32_t playerId, std::string_view message,
                        uWS::OpCode code) const;

private:
  thread t_;
  atomic<bool> running_{false};

  mutex clients_m_;
  mutex pending_m_;
  mutex cb_m_;
  mutex sockets_m_;

  vector<string> pending_;
  std::unordered_map<uint32_t, Ws *> socketsByPlayerId_;
  std::vector<std::vector<uint8_t>> pendingBin_;
  function<void(uint32_t)> onDisconnect_;

  atomic<uWS::Loop *> loop_{nullptr};
  std::atomic<bool> flushScheduled_{false};
  atomic<bool> pumpStarted_{false};
  atomic<int> nextPlayerId_{1};
  function<void(int playerId)> onConnect_;

  us_timer_t *timer_ = nullptr;
  uWS::App *app_ = nullptr;
  atomic<us_listen_socket_t *> listenSocket_{nullptr};

  InBoundQueue &inboundQ;

  const IngressLimits limits;
  IngressStats &stats;

  void run_(int port);

  void scheduleFlush_();

  void flushOnce_();

  static void startPump_();

  void onOpen(Ws *);

  static InBoundMsg makeText(uint32_t playerId, std::string_view message);
  static InBoundMsg makeBinary(uint32_t playerId, std::string_view message);

  void onMessage(Ws *, std::string_view, uWS::OpCode) const;

  static InBoundMsg makeDisconnect(uint32_t player_id);

  void onClose(Ws *, int, std::string_view) const;

  static IngressStats &defaultStats() {
    static IngressStats s; // global-ish, but safe lifetime
    return s;
  }
};

#endif // FOOTBALL_SERVER_UWSHUB_H
