#pragma once

#include "hash.hpp"
#include <array>
#include <string>

namespace utils {
inline std::string generate_klv(const std::string &protocol,
                                const std::string &version, const std::string &rid) {
  constexpr std::array salts = {
      "e9fc40ec08f9ea6393f59c65e37f750aacddf68490c4f92d0d2523a5bc02ea63",
      "c85df9056ee603b849a93e1ebab5dd5f66e1fb8b2f4a8caef8d13b9f9e013fa4",
      "3ca373dffbf463bb337e0fd768a2f395b8e417475438916506c721551f32038d",
      "73eff5914c61a20a71ada81a6fc7780700fb1c0285659b4899bc172a24c14fc1"};

  static std::array constant_values = {sha256(md5(sha256(protocol))),
                                       sha256(sha256(version)),
                                       sha256(sha256(protocol) + salts[3])};

  return sha256(constant_values[0] + salts[0] + constant_values[1] + salts[1] +
                sha256(md5(sha256(rid))) + salts[2] + constant_values[2]);
}

inline uint32_t hash_string(const std::string &input) {
  if (input.empty()) {
    return 0;
  }

  uint32_t acc = 0x55555555;
  for (const auto &byte : input) {
    acc = (acc >> 27) + (acc << 5) + static_cast<uint32_t>(byte);
  }
  return acc;
}
} // namespace utils