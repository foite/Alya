#pragma once
#include "bot/bot.hpp"
#include "fmt/format.h"
#include <magic_enum.hpp>

class Packet {
public:
  static void handle(Bot *bot, uint8_t *data) {
    types::EPacketType packet_type{*(uint32_t *)data};
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
      std::string message = (char *)data;
      spdlog::info("Received game message: {}", message);

      if (message.find("logon_fail") != std::string::npos) {
        bot->info.status = "Failed to log in";
        spdlog::error("Failed to log in");
        bot->disconnect();
      }

      if (message.find("currently banned") != std::string::npos) {
        bot->info.status = "Banned";
        bot->state.is_banned = true;
        spdlog::error("Banned");
        bot->disconnect();
      }

      break;
    }
    default:
      break;
    }
  }
};