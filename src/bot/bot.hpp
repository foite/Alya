#pragma once
#include "bot/botinfo.hpp"
#include "types/e_login_method.hpp"
#include "types/e_packet_type.hpp"
#include "types/tank_packet.hpp"
#include <enet/enet.h>
#include <spdlog/spdlog.h>
#include <string>

class Bot {
public:
  Bot(const std::string &username, const std::string &password,
      const std::string &recovery_code, const types::ELoginMethod method);
  void disconnect();
  void punch();
  void place();
  void warp();
  void talk();
  void send_packet(types::TankPacket packet);
  void send_packet(types::EPacketType packet_type, std::string message);

private:
  void login();
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
  Info info;
  State state;
  Server server;
  Position position;
};
