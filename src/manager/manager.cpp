#include "manager.hpp"

void Manager::add_bot(std::string username, std::string password,
                      std::string recovery_code, types::ELoginMethod method) {
  bots.push_back(std::make_shared<Bot>(std::move(username), std::move(password),
                                       std::move(recovery_code), method));
}
