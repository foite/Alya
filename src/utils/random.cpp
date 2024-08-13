#include "random.hpp"
#include <iomanip>
#include <random>
#include <sstream>

thread_local std::mt19937 generator(std::random_device{}());

std::string utils::random_hex(uint32_t length, bool upper) {
  std::string str = upper ? "0123456789ABCDEF" : "0123456789abcdef";
  std::uniform_int_distribution<> dist(0, str.length() - 1);
  std::string new_str;
  for (uint32_t i = 0; i < length; i++) {
    new_str += str[dist(generator)];
  }
  return new_str;
}

std::string utils::random_mac_address() {
  std::ostringstream mac;
  mac << std::hex << std::setfill('0');
  for (uint32_t i = 0; i < 6; i++) {
    if (i != 0) {
      mac << ":";
    }
    mac << std::setw(2) << random_hex(2, false);
  }
  return mac.str();
}
