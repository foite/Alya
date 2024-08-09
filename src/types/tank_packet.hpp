#pragma once

#include "types/e_tankpacket_type.hpp"

namespace types {
struct TankPacket {
  types::ETankPacketType type;
  uint8_t unk1 = 0;
  uint8_t unk2 = 0;
  uint8_t unk3 = 0;
  uint32_t net_id = 0;
  uint32_t unk4 = 0;
  uint32_t flags = 0;
  uint32_t unk6 = 0;
  uint32_t value = 0;
  float vector_x = 0;
  float vector_y = 0;
  float vector_x2 = 0;
  float vector_y2 = 0;
  float unk12 = 0;
  int32_t int_x = 0;
  int32_t int_y = 0;
  uint32_t extended_data_length = 0;
};
} // namespace types