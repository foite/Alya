#include "inventory.hpp"

void Inventory::parse(uint8_t *data) {
  data += 1; // is this version? i had no idea
  this->size = *reinterpret_cast<uint32_t *>(data);
  data += sizeof(uint32_t);

  this->item_count = *reinterpret_cast<uint16_t *>(data);
  data += sizeof(uint16_t);

  for (uint16_t i = 0; i < this->item_count; i++) {
    InventoryItem item;
    item.id = *reinterpret_cast<uint16_t *>(data);
    data += sizeof(uint16_t);

    item.count = *reinterpret_cast<uint16_t *>(data);
    data += sizeof(uint16_t);

    this->items.push_back(item);
  }
}