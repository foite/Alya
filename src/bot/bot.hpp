#pragma once
#include "bot/botinfo.hpp"
#include "bot/inventory/inventory.hpp"
#include "bot/world/world.hpp"
#include "item/item.hpp"
#include "types/e_login_method.hpp"
#include "types/e_packet_type.hpp"
#include <cstdint>
#include <enet/enet.h>
#include <spdlog/spdlog.h>
#include <string>

class Bot {
public:
  Bot(const std::string &username, const std::string &password,
      const std::string &recovery_code, const types::ELoginMethod method,
      ItemDatabase *item_db);
  void disconnect();
  void punch(int32_t offset_x, int32_t offset_y);
  void place(int32_t offset_x, int32_t offset_y, uint32_t block_id);
  void warp(std::string world_name);
  void talk(std::string message);
  void send_packet(types::EPacketType packet_type, std::string message);
  void login();

private:
  void spoof();
  void to_http();
  void get_token();
  void get_oauth_links();
  void parse_server_data();
  void start_event_loop();
  void create_enet_host();
  void connect_to_server(std::string ip, int16_t port);
  void process_event();
  void sleep();

public:
  std::string user_agent =
      "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, "
      "like Gecko) Chrome/120.0.0.0 Safari/537.36 Edg/120.0.0.0";
  ENetHost *host;
  ENetPeer *peer;
  ItemDatabase *item_db;
  Info info;
  State state;
  Server server;
  Position position;
  World world;
  Inventory inventory;
};
