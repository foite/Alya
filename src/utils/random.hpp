#pragma once

#include <cstdint>
#include <string>

namespace utils {
std::string random_hex(uint32_t length, bool upper = true);
std::string random_mac_address();
} // namespace utils