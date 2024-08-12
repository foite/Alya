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
  uint32_t net_id;
  int32_t gems;
  bool is_running;
  bool is_redirect;
  bool is_ingame;
  bool is_not_allowed_to_warp;
  bool is_banned;
};

struct Server {
  std::string ip = "0.0.0.0";
  int16_t port = 00000;
};

struct Position {
  float_t x = 0;
  float_t y = 0;
};