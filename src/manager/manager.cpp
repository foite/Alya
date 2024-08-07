#include "manager.hpp"
#include "bot/item/item.hpp"

Manager::Manager() {
  this->item_db = parse_from_file("items.dat");
  if (this->item_db == nullptr) {
    spdlog::error("Failed to parse items.dat");
    return;
  }
  spdlog::info("Parsed items.dat");
}

void Manager::add_bot(std::string username, std::string password,
                      std::string recovery_code, types::ELoginMethod method) {
  bots.push_back(std::make_shared<Bot>(std::move(username), std::move(password),
                                       std::move(recovery_code), method,
                                       this->item_db));
}
