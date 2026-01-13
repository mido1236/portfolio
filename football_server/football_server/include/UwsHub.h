// Created by ahmed on 2025-12-30.
//

#ifndef FOOTBALL_SERVER_UWSHUB_H
#define FOOTBALL_SERVER_UWSHUB_H

#pragma once

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

  UwsHub() = default;

  ~UwsHub() { stop(); };

  void start(int port);

  void stop();

  void broadcast(const std::string &text);

  void setOnMessage(std::function<void(uint32_t, std::string_view)> cb);

  void broadcastBinary(std::vector<uint8_t> bytes);

  void setOnBinaryMessage(
      std::function<void(uint32_t, std::span<const uint8_t>)> cb);
  void publishBinary(std::string topic, std::vector<uint8_t> bytes);

  void publishText(string topic, string text);

  void joinMatch(uint32_t playerId, uint32_t matchId);

private:
  thread t_;
  atomic<bool> running_{false};

  mutex clients_m_;
  mutex pending_m_;
  mutex cb_m_;

  vector<Ws *> clients_;
  vector<string> pending_;
  std::unordered_map<int, Ws *> socketsByPlayerId_;
  std::vector<std::vector<uint8_t>> pendingBin_;
  function<void(uint32_t, string_view)> onMessage_;
  function<void(uint32_t, std::span<const uint8_t>)> onBinaryMessage_;
  atomic<uWS::Loop *> loop_{nullptr};
  std::atomic<bool> flushScheduled_{false};
  atomic<bool> pumpStarted_{false};
  atomic<int> nextPlayerId_{1};
  function<void(int playerId)> onConnect_;

  us_timer_t *timer_ = nullptr;
  uWS::App *app_ = nullptr;

  void run_(int port);

  void scheduleFlush_();

  void flushOnce_();

  void startPump_();
};

#endif // FOOTBALL_SERVER_UWSHUB_H
