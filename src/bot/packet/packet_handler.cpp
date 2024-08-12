#include "packet_handler.hpp"
#include "bot/packet/variant_handler.hpp"
#include "fmt/format.h"
#include "types/e_packet_type.hpp"
#include "types/e_tankpacket_type.hpp"
#include "types/tank_packet.hpp"
#include <cstring>
#include <magic_enum.hpp>

void Packet::handle(Bot *bot, uint8_t *data) {
  types::EPacketType packet_type{*reinterpret_cast<uint32_t *>(data)};
  auto name = magic_enum::enum_name(
      magic_enum::enum_value<types::EPacketType>(packet_type));
  spdlog::info("Received packet type: {}", name);

  data += sizeof(types::EPacketType);

  switch (packet_type) {
  case types::EPacketType::NetMessageServerHello: {
    std::string message;

    if (bot->state.is_redirect) {
      message = bot->info.login_info.to_string();
    } else {
      message = fmt::format("protocol|{}\nltoken|{}\nplatformID|{}\n",
                            bot->info.login_info.protocol, bot->info.token,
                            bot->info.login_info.platform_id);
    }
    bot->send_packet(types::EPacketType::NetMessageGenericText, message);
    break;
  }
  case types::EPacketType::NetMessageGameMessage: {
    std::string message = reinterpret_cast<char *>(data);
    spdlog::info("Received game message: {}", message);

    if (message.find("logon_fail") != std::string::npos) {
      bot->info.status = "Failed to log in";
      bot->state.is_redirect = false;
      spdlog::error("Failed to log in");
      bot->disconnect();
    }

    if (message.find("currently banned") != std::string::npos) {
      bot->info.status = "Banned";
      bot->state.is_running = false;
      bot->state.is_banned = true;
      spdlog::error("Banned");
      bot->disconnect();
    }

    if (message.find("temporarily suspended") != std::string::npos) {
      bot->info.status = "Temporarily Suspended";
      bot->state.is_running = false;
      bot->state.is_banned = true;
      spdlog::error("Temporarily Suspended");
      bot->disconnect();
    }

    if (message.find("Advanced Account Protection") != std::string::npos) {
      bot->info.status = "AAP Enabled";
      bot->state.is_running = false;
      spdlog::error("AAP Enabled");
      bot->disconnect();
    }

    break;
  }
  case types::EPacketType::NetMessageGamePacket: {
    types::TankPacket tank_packet;
    memcpy(&tank_packet, data, sizeof(types::TankPacket));

    if (tank_packet.type == types::ETankPacketType::NetGamePacketCallFunction) {
      Variant::handle(bot, data + sizeof(types::TankPacket));
    }
    if (tank_packet.type == types::ETankPacketType::NetGamePacketSendMapData) {
      // It doesn't work fully yet resulting in a crash
      bot->world.parse(data + sizeof(types::TankPacket));
    }
    if (tank_packet.type ==
        types::ETankPacketType::NetGamePacketSendInventoryState) {
      bot->inventory.parse(data + sizeof(types::TankPacket));
    }
    if (tank_packet.type == types::ETankPacketType::NetGamePacketPingRequest) {
      spdlog::info("Received ping request");
      types::TankPacket pkt;
      memcpy(&pkt, data, sizeof(types::TankPacket));
      pkt.type = types::ETankPacketType::NetGamePacketPingReply;
      pkt.net_id = 0;
      pkt.unk2 = 0;
      pkt.vector_x = 64.0;
      pkt.vector_y = 64.0;
      pkt.vector_x2 = 1000.0;
      pkt.vector_y2 = 250.0;

      ENetPacket *enet_packet = enet_packet_create(
          nullptr,
          sizeof(types::EPacketType) + sizeof(types::TankPacket) +
              pkt.extended_data_length,
          ENET_PACKET_FLAG_RELIABLE);
      *reinterpret_cast<types::EPacketType *>(enet_packet->data) =
          types::EPacketType::NetMessageGamePacket;
      memcpy(enet_packet->data + sizeof(types::EPacketType), &pkt,
             sizeof(types::TankPacket));
      memcpy(enet_packet->data + sizeof(types::EPacketType) +
                 sizeof(types::TankPacket),
             data + sizeof(types::TankPacket), pkt.extended_data_length);

      enet_peer_send(bot->peer, 0, enet_packet);
    }
  }
  default:
    break;
  }
}