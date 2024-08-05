#include "bot.hpp"
#include <cpr/cpr.h>
#include <cstring>
#include <fstream>
#include <magic_enum.hpp>
#include <nlohmann/json.hpp>
#include <regex>

#include "enet/enet.h"
#include "packet_handler.hpp"
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
  this->info.status = "Fetching server data";
  spdlog::info("Fetching server data.");
  while (true) {
    cpr::Response r = cpr::Post(
        cpr::Url{"https://www.growtopia1.com/growtopia/server_data.php"},
        cpr::UserAgent{"UbiServices_SDK_2022.Release.9_PC64_ansi_static"},
        cpr::Body{""});

    if (r.status_code != 200) {
      this->info.status = "Failed to fetch server data. Retrying";
      spdlog::error("Failed to fetch server data. Retrying");
      this->sleep();
    } else {
      this->info.status = "Server data fetched";
      spdlog::info("Server data fetched.");
      this->info.server_data = TextParse::parse_and_store_as_map(r.text);
      break;
    }
  }
}

void Bot::sleep() {
  std::ifstream file("config.json");
  nlohmann::json j = nlohmann::json::parse(file);
  this->info.timeout = j["timeout"];
  file.close();
  while (this->info.timeout > 0) {
    this->info.timeout--;
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}

void Bot::start_event_loop() {
  this->info.status = "Starting event loop";
  spdlog::info("Starting event loop");
  this->create_enet_host();
  this->connect_to_server(this->info.server_data["server"],
                          std::stoi(this->info.server_data["port"]));

  while (this->state.is_running) {
    this->process_event();
  }
}

void Bot::process_event() {
  ENetEvent event;
  while (enet_host_service(this->host, &event, 1000)) {
    switch (event.type) {
    case ENET_EVENT_TYPE_CONNECT:
      this->info.status = "Connected";
      spdlog::info("Connected to server");
      break;
    case ENET_EVENT_TYPE_RECEIVE:
      spdlog::info("Received a packet");
      if (event.packet->dataLength < 4) {
        break;
      }
      Packet::handle(this, event.packet->data);
      break;
    case ENET_EVENT_TYPE_DISCONNECT:
      spdlog::info("Disconnected from server");
      if (this->state.is_redirect) {
        this->connect_to_server(this->server.ip, this->server.port);
      } else {
        this->to_http();
        this->connect_to_server(this->info.server_data["server"],
                                std::stoi(this->info.server_data["port"]));
      }
      break;
    case ENET_EVENT_TYPE_NONE:
      break;
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
  this->info.status = "Spoofing data";
  spdlog::info("Spoofing data");
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
  this->info.status = "Getting OAuth links";
  spdlog::info("Getting OAuth links");
  while (true) {
    cpr::Response r = cpr::Post(
        cpr::Url{"https://login.growtopiagame.com/player/login/dashboard"},
        cpr::UserAgent{this->user_agent},
        cpr::Body{this->info.login_info.to_string()});
    const std::regex pattern(
        "https:\\/\\/login\\.growtopiagame\\.com\\/(apple|google|player\\/"
        "growid)\\/(login|redirect)\\?token=[^\"]+");

    if (r.status_code != 200) {
      this->info.status = "Failed to fetch OAuth links. Retrying";
      spdlog::error("Failed to fetch OAuth links. Retrying");
      this->sleep();
    } else {
      this->info.status = "OAuth links fetched";
      spdlog::info("OAuth links fetched");
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
      break;
    }
  }
}

void Bot::send_packet(types::EPacketType packet_type, std::string &message) {
  ENetPacket *packet =
      enet_packet_create(nullptr, sizeof(types::EPacketType) + message.length(),
                         ENET_PACKET_FLAG_RELIABLE);
  *(types::EPacketType *)packet->data = packet_type;
  memcpy(packet->data + sizeof(types::EPacketType), message.c_str(),
         message.length());

  enet_peer_send(this->peer, 0, packet);
}