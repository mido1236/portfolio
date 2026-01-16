//
// Created by ahmed on 2025-12-30.
//

#define _HAS_STD_BYTE 0
#include <Topics.h>
#include <UwsHub.h>
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
    l->defer([this, l] {
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

void UwsHub::setOnBinaryMessage(
    std::function<void(uint32_t, std::span<const uint8_t>)> cb) {
  lock_guard lock(cb_m_);
  onBinaryMessage_ = std::move(cb);
}

void UwsHub::publishBinary(std::string topic, std::vector<uint8_t> bytes) {
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

void UwsHub::setOnMessage(std::function<void(uint32_t, std::string_view)> cb) {
  lock_guard lock(cb_m_);
  onMessage_ = move(cb);
}

void UwsHub::joinMatch(const uint32_t playerId, const uint32_t matchId) {
  const auto ws = socketsByPlayerId_[playerId];

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

void UwsHub::run_(int port) {
  uWS::App app;
  app_ = &app;

  loop_.store(uWS::Loop::get(), memory_order::release);

  app.ws<PerSocketData>(
      "/*",
      {.open =
           [this](Ws *ws) {
             const int id = nextPlayerId_.fetch_add(1, memory_order::relaxed);
             ws->getUserData()->playerId = id;
             socketsByPlayerId_[id] = ws;
             ws->getUserData()->matchId = 0;

             ws->send(R"( {"type":"welcome","cmd":"JOIN <matchId>"} )",
                      uWS::OpCode::TEXT);

             cout << "Client connected (" << clients_.size() << ")" << endl;
           },
       .message =
           [this](Ws *ws, const std::string_view message,
                  const uWS::OpCode code) {
             if (code == uWS::OpCode::TEXT) {
               function<void(uint32_t, string_view)> cb;
               {
                 lock_guard lock(cb_m_);
                 cb = onMessage_;
               }
               if (cb)
                 cb(ws->getUserData()->playerId, message);
             } else if (code == uWS::OpCode::BINARY) {
               function<void(uint32_t, std::span<const uint8_t>)> cb;
               {
                 lock_guard lock(cb_m_);
                 cb = onBinaryMessage_;
               }
               if (!cb)
                 return;
               const auto *data =
                   reinterpret_cast<const uint8_t *>(message.data());
               cb(ws->getUserData()->playerId, std::span(data, message.size()));
             }
           },
       .close =
           [this](Ws *ws, int, std::string_view) {
             lock_guard lock(clients_m_);
             clients_.erase(std::ranges::remove(clients_, ws).begin(),
                            clients_.end());
             cout << "Client disconnected (" << clients_.size() << ")" << endl;
           }});

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
