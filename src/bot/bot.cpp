#include "bot.hpp"
#include <cpr/cpr.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <magic_enum.hpp>
#include <nlohmann/json.hpp>
#include <regex>

#include "bot/login/login.hpp"
#include "enet/enet.h"
#include "packet/packet_handler.hpp"
#include "spdlog/spdlog.h"
#include "types/e_tankpacket_type.hpp"
#include "types/tank_packet.hpp"
#include "utils/proton.hpp"
#include "utils/textparse.hpp"

Bot::Bot(const std::string &username, const std::string &password,
         const std::string &recovery_code, const types::ELoginMethod method,
         ItemDatabase *item_db) {
  this->info.username = std::move(username);
  this->info.password = std::move(password);
  this->info.recovery_code = std::move(recovery_code);
  this->info.method = method;
  this->item_db = item_db;
}

void Bot::login() {
  if (enet_initialize() != 0) {
    spdlog::error("An error occurred while initializing ENet.");
  } else {
    spdlog::info("ENet initialized.");
  }
  this->state.is_running = true;
  this->to_http();
  this->spoof();
  this->get_oauth_links();
  this->get_token();
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
      this->info.server_data = utils::TextParse::parse_and_store_as_map(r.text);
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

  while (this->state.is_running && !this->state.is_banned) {
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
      this->info.ping = event.peer->lastRoundTripTime;
      if (event.packet->dataLength < 4) {
        break;
      }
      Packet::handle(this, event.packet->data);
      enet_packet_destroy(event.packet);
      break;
    case ENET_EVENT_TYPE_DISCONNECT:
      this->info.ping = 0;
      spdlog::info("Disconnected from server");
      if (this->state.is_running) {
        if (this->state.is_redirect) {
          spdlog::info("Redirecting to server");
          this->connect_to_server(this->server.ip, this->server.port);
        } else {
          this->to_http();
          this->connect_to_server(this->info.server_data["server"],
                                  std::stoi(this->info.server_data["port"]));
        }
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
    std::string encoded_body = cpr::util::urlEncode(this->info.login_info.to_string());
    cpr::Response r = cpr::Post(
        cpr::Url{"https://login.growtopiagame.com/player/login/dashboard?valKey=40db4045f2d8c572efe8c4a060605726"},
        cpr::UserAgent{this->user_agent},
        cpr::Body{encoded_body},
        cpr::Header{{"Content-Type", "application/x-www-form-urlencoded"}});
    const std::regex pattern(
        "https:\\/\\/login\\.growtopiagame\\.com\\/(apple|google|player\\/"
        "growid)\\/(login|redirect)\\?token=[^\"]+");

    if (r.status_code != 200) {
      this->info.status = "Failed to fetch OAuth links. Retrying";
      spdlog::error("Failed to fetch OAuth links. Retrying");
      this->sleep();
      continue;
    } else {
      if (r.text.find("Oops, too many people logging at once.") !=
          std::string::npos) {
        this->info.status = "Too many people logging at once. Retrying";
        spdlog::error("Too many people logging at once. Retrying");
        this->sleep();
        continue;
      }
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

void Bot::get_token() {
  this->info.status = "Getting token";
  spdlog::info("Getting token");

  switch (this->info.method) {
  case types::ELoginMethod::LEGACY: {
    this->info.token =
        Login::get_legacy_token(this->info.oauth_links["legacy"],
                                this->info.username, this->info.password);
    break;
  default:
    break;
  }
  }

  spdlog::info("Token: {}", this->info.token);
}

void Bot::disconnect() { enet_peer_disconnect(this->peer, 0); }

void Bot::place(int32_t offset_x, int32_t offset_y, uint32_t block_id) {
  types::TankPacket pkt;
  pkt.type = types::ETankPacketType::NetGamePacketTileChangeRequest;
  pkt.vector_x = this->position.x;
  pkt.vector_y = this->position.y;
  pkt.int_x = std::floor(this->position.x / 32.0) + offset_x;
  pkt.int_y = std::floor(this->position.y / 32.0) + offset_y;
  pkt.value = block_id;

  if (pkt.int_x <= std::floor(this->position.x / 32.0) + 4 &&
      pkt.int_x >= std::floor(this->position.x / 32.0) - 4 &&
      pkt.int_y <= std::floor(this->position.y / 32.0) + 4 &&
      pkt.int_y >= std::floor(this->position.y / 32.0) - 4) {
    ENetPacket *enet_packet = enet_packet_create(nullptr,
                                                 sizeof(types::EPacketType) +
                                                     sizeof(types::TankPacket) +
                                                     pkt.extended_data_length,
                                                 ENET_PACKET_FLAG_RELIABLE);
    *(types::EPacketType *)enet_packet->data =
        types::EPacketType::NetMessageGamePacket;
    memcpy(enet_packet->data + sizeof(types::EPacketType), &pkt,
           sizeof(types::TankPacket));

    enet_peer_send(this->peer, 0, enet_packet);
  }
}

void Bot::punch(int32_t offset_x, int32_t offset_y) {
  this->place(offset_x, offset_y, 0);
}

void Bot::warp(std::string world_name) {
  spdlog::info("Warping to world: {}", world_name);
  this->send_packet(
      types::EPacketType::NetMessageGameMessage,
      fmt::format("action|join_request\nname|{}\ninvitedWorld|0\n",
                  world_name));
}

void Bot::talk(std::string message) {
  this->send_packet(types::EPacketType::NetMessageGameMessage,
                    fmt::format("action|input\n|text|{}\n", message));
}

// TODO: Fix Ban
void Bot::walk(int32_t x, int32_t y, bool ap) {
  if (ap) {
    this->position.x = x * 32;
    this->position.y = y * 32;
  } else {
    this->position.x += x * 32;
    this->position.y += y * 32;
  }

  types::TankPacket pkt;
  pkt.type = types::ETankPacketType::NetGamePacketState;
  pkt.vector_x = this->position.x;
  pkt.vector_y = this->position.y;
  pkt.int_x = -1;
  pkt.int_y = -1;
  // I'm not sure why this resulted into a ban, let me know if you had any idea
  // pkt.flags |= 1 << 1;
  // pkt.flags |= 1 << 5;
  pkt.flags = 0;

  if (this->state.is_running && this->world.name != "EXIT") {
    ENetPacket *enet_packet = enet_packet_create(nullptr,
                                                 sizeof(types::EPacketType) +
                                                     sizeof(types::TankPacket) +
                                                     pkt.extended_data_length,
                                                 ENET_PACKET_FLAG_RELIABLE);
    *(types::EPacketType *)enet_packet->data =
        types::EPacketType::NetMessageGamePacket;
    memcpy(enet_packet->data + sizeof(types::EPacketType), &pkt,
           sizeof(types::TankPacket));

    enet_peer_send(this->peer, 0, enet_packet);
  }
}

void Bot::send_packet(types::EPacketType packet_type, std::string message) {
  ENetPacket *packet =
      enet_packet_create(nullptr, sizeof(types::EPacketType) + message.length(),
                         ENET_PACKET_FLAG_RELIABLE);
  *(types::EPacketType *)packet->data = packet_type;
  memcpy(packet->data + sizeof(types::EPacketType), message.c_str(),
         message.length());

  enet_peer_send(this->peer, 0, packet);
}