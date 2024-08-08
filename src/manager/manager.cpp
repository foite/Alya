#include "manager.hpp"
#include "bot/item/item.hpp"
#include "nlohmann/json.hpp"
#include <fstream>

Manager::Manager() {
  this->item_db = parse_from_file("items.dat");
  if (this->item_db == nullptr) {
    spdlog::error("Failed to parse items.dat");
    return;
  }
  spdlog::info("Parsed items.dat");

  std::ifstream file("config.json");
  nlohmann::json j = nlohmann::json::parse(file);
  for (auto &bot : j["bots"]) {
    this->add_bot(bot[0], bot[1], bot[2], bot[3]);
  }
  file.close();
}

Manager::~Manager() {
  for (auto &[username, bot_thread_tuple] : bots) {
    auto &[bot, thread] = bot_thread_tuple;
    if (thread.joinable()) {
      pthread_cancel(thread.native_handle());
      thread.join();
    }
  }
}

void Manager::add_bot(std::string username, std::string password,
                      std::string recovery_code, types::ELoginMethod method,
                      bool save) {
  if (save) {
    std::ifstream file("config.json");
    nlohmann::json j = nlohmann::json::parse(file);
    file.close();
    j["bots"].push_back({username, password, recovery_code, method});
    std::ofstream out("config.json");
    out << j.dump(2);
    out.close();
  }

  auto bot =
      std::make_shared<Bot>(std::move(username), std::move(password),
                            std::move(recovery_code), method, this->item_db);
  std::thread t([bot]() { bot->login(); });
  this->bots[bot->info.username] = std::make_tuple(bot, std::move(t));
}

std::shared_ptr<Bot> Manager::get_bot(std::string username) {
  auto it = bots.find(username);
  if (it != bots.end()) {
    return std::get<0>(it->second);
  }
  return nullptr;
}

void Manager::remove_bot(std::string username) {
  auto it = bots.find(username);
  if (it != bots.end()) {
    auto &[bot, thread] = it->second;
    if (thread.joinable()) {
      pthread_cancel(thread.native_handle());
      thread.join();
    }
    bots.erase(it);
  }
}
