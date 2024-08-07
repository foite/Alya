#pragma once
#include <cstdint>
#include <vector>

struct InventoryItem {
  uint16_t id;
  uint16_t count;
};

class Inventory {
public:
  uint32_t size;
  uint16_t item_count;
  std::vector<InventoryItem> items;

  void parse(uint8_t *data);
};