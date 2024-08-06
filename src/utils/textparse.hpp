#pragma once
#include <string>
#include <unordered_map>
#include <vector>

namespace utils {
class TextParse {
public:
  static std::vector<std::string> parse_and_store_as_vec(std::string &input);
  static std::unordered_map<std::string, std::string>
  parse_and_store_as_map(std::string &input);
};
} // namespace utils