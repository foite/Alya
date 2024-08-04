#pragma once
#include <string>
#include <vector>
#include <map>

class TextParse {
public:
	static std::vector<std::string> parse_and_store_as_vec(std::string& input);
	static std::map<std::string, std::string> parse_and_store_as_map(std::string& input);
};