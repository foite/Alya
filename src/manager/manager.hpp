#pragma once
#include "bot/bot.hpp"
#include "bot/item/item.hpp"
#include "types/e_login_method.hpp"
#include <memory>
#include <string>
#include <unordered_map>

class Manager {
public:
  Manager();
  ~Manager();
  void add_bot(std::string username, std::string password,
               std::string recovery_code, types::ELoginMethod method);
  std::shared_ptr<Bot> get_bot(std::string username);
  void remove_bot(std::string username);

  std::unordered_map<std::string, std::tuple<std::shared_ptr<Bot>, std::thread>>
      bots;
  ItemDatabase *item_db;
};