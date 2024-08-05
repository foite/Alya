#pragma once

#include <string>

namespace utils {
static std::string random_hex(uint32_t length, bool upper = true) {
  std::string str = "0123456789abcdef";
  if (upper) {
    str = "0123456789ABCDEF";
  }
  std::string new_str;
  for (uint32_t i = 0; i < length; i++) {
    new_str += str[rand() % str.length()];
  }

  return new_str;
}

static std::string random_mac_address() {
  std::string mac = random_hex(2, false);
  for (uint32_t i = 0; i < 5; i++) {
    mac += ":" + random_hex(2, false);
  }
  return mac;
}
} // namespace utils