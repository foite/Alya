#include "textparse.hpp"

std::string delimiter = "|";

std::vector<std::string>
utils::TextParse::parse_and_store_as_vec(std::string &input) {
  std::vector<std::string> result;
  size_t pos = 0;

  while ((pos = input.find(delimiter)) != std::string::npos) {
    std::string token = input.substr(0, pos);
    result.push_back(token);
    input.erase(0, pos + delimiter.length());
  }

  result.push_back(input);

  return result;
}

std::unordered_map<std::string, std::string>
utils::TextParse::parse_and_store_as_map(std::string &input) {

  std::unordered_map<std::string, std::string> result;
  size_t pos = 0;

  while ((pos = input.find(delimiter)) != std::string::npos) {
    const std::string key = input.substr(0, pos);
    input.erase(0, pos + 1);
    pos = input.find('\n');
    const std::string value = input.substr(0, pos);
    result[key] = value;
    input.erase(0, pos + 1);
  }

  return result;
}
