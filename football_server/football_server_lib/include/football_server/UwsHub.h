// Created by ahmed on 2025-12-30.
//

#ifndef FOOTBALL_SERVER_UWSHUB_H
#define FOOTBALL_SERVER_UWSHUB_H

#pragma once

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

class UwsHub {
public:
  using Ws = uWS::WebSocket<false, true, PerSocketData>;

  explicit UwsHub(InBoundQueue &q) : inboundQ(q) {}

  ~UwsHub() { stop(); };

  void start(int port);

  void stop();

  void broadcast(const std::string &text);

  void broadcastBinary(std::vector<uint8_t> bytes);

  void publishBinary(std::string topic, std::vector<uint8_t> bytes);

  void publishText(string topic, string text) const;

  void joinMatch(uint32_t playerId, uint32_t matchId);

private:
  thread t_;
  atomic<bool> running_{false};

  mutex clients_m_;
  mutex pending_m_;
  mutex cb_m_;
  mutex sockets_m_;

  vector<Ws *> clients_;
  vector<string> pending_;
  std::unordered_map<uint32_t, Ws *> socketsByPlayerId_;
  std::vector<std::vector<uint8_t>> pendingBin_;
  function<void(uint32_t, string_view)> onMessage_;
  function<void(uint32_t, std::span<const uint8_t>)> onBinaryMessage_;
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

  void run_(int port);

  void scheduleFlush_();

  void flushOnce_();

  void startPump_();

  void onOpen(Ws *);

  void onMessage(Ws *, std::string_view, uWS::OpCode);

  void onClose(Ws *, int, std::string_view);
};

#endif // FOOTBALL_SERVER_UWSHUB_H
