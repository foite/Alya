#pragma once
#include "bot/bot.hpp"

class Packet {
public:
  static void handle(Bot *bot, uint8_t *data);
};