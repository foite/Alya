#pragma once
#include "bot/bot.hpp"
#include "fmt/format.h"
#include <magic_enum.hpp>

class Packet {
public:
  static void handle(Bot *bot, uint8_t *data);
};