#pragma once
#include "bot/bot.hpp"
#include "bot/item/item.hpp"
#include "types/e_login_method.hpp"
#include <memory>
#include <string>
#include <vector>

class Manager {
public:
  Manager();
  void add_bot(std::string username, std::string password,
               std::string recovery_code, types::ELoginMethod method);
  void get_bot(std::string username);
  void remove_bot(std::string username);

  std::vector<std::shared_ptr<Bot>> bots;
  ItemDatabase *item_db;
};