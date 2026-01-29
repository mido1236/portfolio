//
// Created by ahmed on 2025-12-30.
//

#define _HAS_STD_BYTE 0
#include <football_server/Topics.h>
#include <football_server/UwsHub.h>
#include <span>

using std::cout;
using std::endl;
using std::lock_guard;
using std::memory_order;

void UwsHub::start(int port) {
  if (running_.exchange(true))
    return;
  t_ = thread([this, port] { run_(port); });
}

void UwsHub::stop() {
  if (!running_.exchange(false))
    return;
  if (auto *l = loop_.load(memory_order::acquire)) {
    l->defer([this] {
      if (timer_) {
        us_timer_close(timer_);
        timer_ = nullptr;
      }
      if (auto *ls = listenSocket_.exchange(nullptr, memory_order::acquire)) {
        us_listen_socket_close(0, ls);
      }
    });
  }
  if (t_.joinable())
    t_.join();
}

void UwsHub::broadcast(const std::string &text) {
  // Check messages
  {
    lock_guard lock(pending_m_);
    pending_.push_back(move(text));
  }
  scheduleFlush_();
}

void UwsHub::broadcastBinary(std::vector<uint8_t> bytes) {
  {
    lock_guard lock(pending_m_);
    pendingBin_.push_back(move(bytes));
  }
  scheduleFlush_();
}

void UwsHub::publishBinary(std::string topic,
                           std::vector<uint8_t> bytes) const {
  auto *l = loop_.load(std::memory_order_acquire);
  if (!l)
    return;

  l->defer([this, topic = std::move(topic),
            bytes = std::move(bytes)]() mutable {
    if (!app_)
      return;
    app_->publish(topic,
                  std::string_view(reinterpret_cast<const char *>(bytes.data()),
                                   bytes.size()),
                  uWS::OpCode::BINARY);
  });
}

void UwsHub::publishText(string topic, string text) {
  if (auto *l = loop_.load(memory_order::acquire)) {
    l->defer([this, topic = std::move(topic), text = std::move(text)] {
      if (!app_)
        return;
      app_->publish(topic, text, uWS::OpCode::TEXT);
    });
  };
}

void UwsHub::joinMatch(const uint32_t playerId, const uint32_t matchId) {
  uWS::WebSocket<false, true, PerSocketData> *ws;

  {
    lock_guard lock(sockets_m_);
    ws = socketsByPlayerId_[playerId];
  }

  if (ws == nullptr)
    return;

  if (matchId == -1) {
    ws->send(R"({"type":"error","msg":"Bad JOIN. Use: JOIN <matchId>"})",
             uWS::OpCode::TEXT);
  }

  const auto ud = ws->getUserData();

  if (ud->matchId != 0) {
    ws->unsubscribe(matchTopic(ud->matchId));
  }

  ud->matchId = matchId;
  ws->subscribe(matchTopic(matchId));
  ws->send(R"({"type":"joined","matchId":)" + std::to_string(matchId) + "}",
           uWS::OpCode::TEXT);
}

void UwsHub::onOpen(Ws *ws) {
  const int id = nextPlayerId_.fetch_add(1, memory_order::relaxed);
  ws->getUserData()->playerId = id;
  {
    lock_guard lock(sockets_m_);
    socketsByPlayerId_[id] = ws;
  }
  ws->getUserData()->matchId = 0;

  ws->send(R"( {"type":"welcome","playerId":")" + std::to_string(id) +
               R"(","cmd":"JOIN <matchId>"} )",
           uWS::OpCode::TEXT);

  cout << "Client connected (" << id << ")" << endl;
}

InBoundMsg UwsHub::makeText(const uint32_t playerId,
                            const std::string_view message) {
  return {playerId, MsgType::Text, string(message)};
}

InBoundMsg UwsHub::makeBinary(const uint32_t playerId,
                              const std::string_view message) {
  const auto *data = reinterpret_cast<const uint8_t *>(message.data());
  return {playerId, MsgType::Binary, "", vector(data, data + message.size())};
}

void UwsHub::onMessageIngress(const uint32_t playerId,
                              const std::string_view message,
                              const uWS::OpCode code) const {
  switch (code) {
  case uWS::OpCode::TEXT:
    if (message.size() > limits.maxTextBytes)
      ++stats.drop_text_too_large;
    else if (!inboundQ.push(makeText(playerId, message)))
      ++stats.drop_queue_full;
    break;
  case uWS::OpCode::BINARY:
    if (message.size() > limits.maxBinaryBytes)
      ++stats.drop_bin_too_large;
    else if (!inboundQ.push(makeBinary(playerId, message)))
      ++stats.drop_queue_full;
    break;
  default:;
    ++stats.drop_unsupported_opcode;
  }
}
void UwsHub::onMessage(Ws *ws, const std::string_view message,
                       const uWS::OpCode code) const {
  const auto playerId = static_cast<uint32_t>(ws->getUserData()->playerId);

  onMessageIngress(playerId, message, code);
}

InBoundMsg UwsHub::makeDisconnect(const uint32_t player_id) {
  return {player_id,
          MsgType::Disconnect,
          "",
          {},
          DisconnectReason::ClosedByPeer,
          0,
          "Connection closed by peer"};
}

void UwsHub::onClose(Ws *ws, int, std::string_view) const {
  const auto playerId = static_cast<uint32_t>(ws->getUserData()->playerId);

  cout << "Client disconnected (" << playerId << ")" << endl;
  inboundQ.push(makeDisconnect(playerId));
}

void UwsHub::run_(int port) {
  uWS::App app;
  app_ = &app;

  loop_.store(uWS::Loop::get(), memory_order::release);

  app.ws<PerSocketData>(
      "/*",
      {.open = [this](Ws *ws) { onOpen(ws); },
       .message =
           [this](Ws *ws, const std::string_view message,
                  const uWS::OpCode code) { onMessage(ws, message, code); },
       .close = [this](Ws *ws, int, std::string_view) { onClose(ws, 0, ""); }});

  app.listen(port, [&, port](us_listen_socket_t *token) {
    listenSocket_.store(token, std::memory_order::release);
    if (token)
      std::cout << "[uWS] listening on port " << port << "\n";
    else
      std::cout << "[uWS] listen failed on port " << port << "\n";
  });

  app.run();

  listenSocket_.store(nullptr, std::memory_order::release);
  loop_.store(nullptr, memory_order::release);
  app_ = nullptr;
}

void UwsHub::scheduleFlush_() {
  auto *l = loop_.load(std::memory_order_acquire);
  if (!l)
    return;

  // Only schedule if not already scheduled
  bool expected = false;
  if (flushScheduled_.compare_exchange_strong(expected, true,
                                              std::memory_order_acq_rel)) {
    l->defer([this] { flushOnce_(); });
  }
}

void UwsHub::flushOnce_() {
  if (!app_)
    return;

  flushScheduled_.store(false, std::memory_order_release);

  vector<string> toSend;
  vector<vector<uint8_t>> toSendBin;

  // Prepare data to send
  {
    lock_guard lock(pending_m_);
    if (pending_.empty() && pendingBin_.empty())
      return;
    toSend.swap(pending_);
    toSendBin.swap(pendingBin_);
  }

  for (const auto &s : toSend)
    app_->publish("snapshots", string_view(s), uWS::OpCode::TEXT);
  for (const auto &b : toSendBin)
    app_->publish(
        "snapshots",
        string_view(reinterpret_cast<const char *>(b.data()), b.size()),
        uWS::OpCode::BINARY);

  // Re-flush if needed
  {
    lock_guard lock(pending_m_);
    if (pending_.empty() && pendingBin_.empty())
      return;
    scheduleFlush_();
  }
}

void UwsHub::startPump_() {}
