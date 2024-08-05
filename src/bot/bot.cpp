#include "bot.hpp"
#include <cpr/cpr.h>
#include <regex>
#include <spdlog/spdlog.h>

#include "enet/enet.h"
#include "utils/proton.hpp"
#include "utils/textparse.hpp"

Bot::Bot(const std::string &username, const std::string &password,
         const std::string &recovery_code, const types::ELoginMethod method) {
  this->info.username = std::move(username);
  this->info.password = std::move(password);
  this->info.recovery_code = std::move(recovery_code);
  this->info.method = method;

  this->login();
}

void Bot::login() {
  if (enet_initialize() != 0) {
    spdlog::error("An error occurred while initializing ENet.");
  }
  this->state.is_running = true;
  this->to_http();
  this->spoof();
  this->get_oauth_links();
  this->start_event_loop();
}

void Bot::to_http() {
  cpr::Response r = cpr::Post(
      cpr::Url{"https://www.growtopia1.com/growtopia/server_data.php"},
      cpr::UserAgent{"UbiServices_SDK_2022.Release.9_PC64_ansi_static"},
      cpr::Body{""});
  this->info.server_data = TextParse::parse_and_store_as_map(r.text);
}

void Bot::start_event_loop() {
  this->create_enet_host();
  while (this->state.is_running) {
    if (this->state.is_redirect) {
      this->connect_to_server(this->server.ip, this->server.port);
    } else {
      if (this->state.is_ingame) {
        // TODO: Get token again
      }
      this->to_http();
      this->connect_to_server(this->info.server_data["server"],
                              std::stoi(this->info.server_data["port"]));
    }
    this->process_event();
  }
}

void Bot::process_event() {
  while (true) {
    ENetEvent event;
    while (enet_host_service(this->host, &event, 1000)) {
      switch (event.type) {
      case ENET_EVENT_TYPE_CONNECT:
        spdlog::info("Connected to server.");
        break;
      case ENET_EVENT_TYPE_RECEIVE:
        spdlog::info("Received a packet.");
        break;
      case ENET_EVENT_TYPE_DISCONNECT:
        spdlog::info("Disconnected from server.");
        break;
      case ENET_EVENT_TYPE_NONE:
        break;
      }
    }
  }
}

void Bot::create_enet_host() {
  this->host = enet_host_create(nullptr, 1, 1, 0, 0);
  if (this->host == nullptr) {
    spdlog::error("An error occurred while creating an ENet host.");
  }
  this->host->usingNewPacket = true;
  this->host->checksum = enet_crc32;
  enet_host_compress_with_range_coder(this->host);
}

void Bot::connect_to_server(std::string ip, int16_t port) {
  ENetAddress address;
  enet_address_set_host(&address, ip.c_str());
  address.port = port;

  this->peer = enet_host_connect(this->host, &address, 2, 0);
}

void Bot::spoof() {
  this->info.login_info.klv = utils::generate_klv(
      this->info.login_info.protocol, this->info.login_info.game_version,
      this->info.login_info.rid);
  this->info.login_info.hash = std::to_string(
      utils::hash_string(fmt::format("{}RT", this->info.login_info.mac)));
  this->info.login_info.hash2 = std::to_string(
      utils::hash_string(fmt::format("{}RT", this->info.login_info.rid)));
  this->info.login_info.meta = this->info.server_data["meta"];
}

void Bot::get_oauth_links() {
  cpr::Response r = cpr::Post(
      cpr::Url{"https://login.growtopiagame.com/player/login/dashboard"},
      cpr::UserAgent{this->user_agent},
      cpr::Body{this->info.login_info.to_string()});
  const std::regex pattern(
      "https:\\/\\/login\\.growtopiagame\\.com\\/(apple|google|player\\/"
      "growid)\\/(login|redirect)\\?token=[^\"]+");

  auto matches_begin =
      std::sregex_iterator(r.text.begin(), r.text.end(), pattern);
  auto matches_end = std::sregex_iterator();

  for (std::sregex_iterator i = matches_begin; i != matches_end; ++i) {
    const std::smatch &match = *i;
    if (match.str().find("apple") != std::string::npos) {
      this->info.oauth_links["apple"] = match.str();
    } else if (match.str().find("google") != std::string::npos) {
      this->info.oauth_links["google"] = match.str();
    } else {
      this->info.oauth_links["legacy"] = match.str();
    }
  }
}
