//
// Created by ahmed on 2026-01-21.
//

#ifndef FOOTBALL_SERVER_LIB_MESSAGES_H
#define FOOTBALL_SERVER_LIB_MESSAGES_H
#include <cstdint>
#include <span>
#include <string_view>
#include <vector>

enum class MsgType : uint8_t { Text, Binary, Disconnect };

enum class DisconnectReason : uint8_t {
  ClosedByPeer,
  ClosedByServer,
  Error,
  Unknown
};

struct InBoundMsg {
  uint32_t playerId{0};
  MsgType type;

  // For text/binary
  std::string text;
  std::vector<uint8_t> bytes;

  // For disconnect
  DisconnectReason reason{DisconnectReason::Unknown};
  int closeCode{0};
  std::string closeMessage;

  static InBoundMsg makeBinary(const uint32_t pid, const uint8_t *data,
                               const size_t size) {
    InBoundMsg m;
    m.playerId = pid;
    m.type = MsgType::Binary;
    m.bytes.assign(data, data + size); // deep copy
    return m;
  }
};
#endif // FOOTBALL_SERVER_LIB_MESSAGES_H
