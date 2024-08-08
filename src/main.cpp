#include "manager/manager.hpp"
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using namespace types;

void init_config() {
  if (!std::filesystem::exists("config.json")) {
    std::ofstream file("config.json");
    nlohmann::json j;
    j["bots"] = nlohmann::json::array();
    j["timeout"] = 15;

    file << j.dump(2);
    file.close();
    spdlog::info("Config file created");
  }
}

int main() {
  init_config();
  Manager manager{};
  manager.add_bot("", ".", "", ELoginMethod::LEGACY);
  return 0;
}