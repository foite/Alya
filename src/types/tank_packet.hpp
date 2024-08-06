#pragma once

#include "types/e_tankpacket_type.hpp"

namespace types {
struct TankPacket {
  types::ETankPacketType type;
  uint8_t unk1;
  uint8_t unk2;
  uint8_t unk3;
  uint32_t net_id;
  uint32_t unk4;
  uint32_t flags;
  uint32_t unk6;
  uint32_t value;
  float vector_x;
  float vector_y;
  float vector_x2;
  float vector_y2;
  float unk12;
  int32_t int_x;
  int32_t int_y;
  uint32_t extended_data_length;
};
} // namespace types