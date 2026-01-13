//
// Created by ahmed on 2026-01-05.
//

#ifndef FOOTBALL_SERVER_PARSING_H
#define FOOTBALL_SERVER_PARSING_H
#include <cstdint>
#include <span>

static inline uint16_t read_le_u16(const uint8_t *p) {
  return static_cast<uint16_t>(p[0]) | (static_cast<uint16_t>(p[1]) << 8);
}

static inline uint32_t read_le_u32(const uint8_t *p) {
  return static_cast<uint32_t>(p[0]) | (static_cast<uint32_t>(p[1]) << 8) |
         (static_cast<uint32_t>(p[2]) << 16) |
         (static_cast<uint32_t>(p[3]) << 24);
}

static inline int16_t read_le_i16(const uint8_t *p) {
  return static_cast<int16_t>(read_le_u16(p));
}

static inline float norm_axis_i16(const int16_t v) {
  // maps [-32768, 32767] to ~[-1, 1]
  // keep symmetric-ish and avoid division by 0
  if (v <= -32768)
    return -1.f;
  return static_cast<float>(v) / 32767.f;
}

static bool parseInputCmd(const std::span<const uint8_t> bytes, InputCmd &out) {
  if (bytes.size() < 16)
    return false;

  out.matchId = read_le_u32(bytes.data() + 0);
  out.clientTick = read_le_u32(bytes.data() + 4);

  const int16_t ax16 = read_le_i16(bytes.data() + 8);
  const int16_t ay16 = read_le_i16(bytes.data() + 10);

  out.ax = norm_axis_i16(ax16);
  out.ay = norm_axis_i16(ay16);

  out.buttons = read_le_u16(bytes.data() + 12);
  return true;
}

#endif // FOOTBALL_SERVER_PARSING_H
