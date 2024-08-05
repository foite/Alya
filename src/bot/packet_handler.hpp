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
    default:
      break;
    }
  }
};