#pragma once
#include "types/e_login_method.hpp"
#include "types/login_info.hpp"
#include <cmath>
#include <cstdint>
#include <string>
#include <unordered_map>

struct Info {
  std::string display_name;
  std::string username;
  std::string password;
  std::string recovery_code;
  types::ELoginMethod method;
  std::unordered_map<std::string, std::string> oauth_links;
  std::unordered_map<std::string, std::string> server_data;
  std::string token;
  types::LoginInfo login_info;
  int32_t ping = 0;
  std::string status;
  int32_t timeout = 0;
};

struct State {
  uint32_t net_id = 0;
  int32_t gems = 0;
  bool is_running = false;
  bool is_redirect = false;
  bool is_ingame = false;
  bool is_not_allowed_to_warp = false;
  bool is_banned = false;
};

struct Server {
  std::string ip = "0.0.0.0";
  int16_t port = 00000;
};

struct Position {
  float_t x = 0;
  float_t y = 0;
};