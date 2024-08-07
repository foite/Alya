#pragma once
#include <array>
#include <cstdint>
#include <string>
#include <variant>
#include <vector>

struct Door {
  std::string text;
  uint8_t unknown_1;
};

struct Sign {
  std::string text;
  uint32_t unknown_1;
};

struct Lock {
  uint8_t settings;
  uint32_t owner_uid;
  uint32_t access_count;
  std::vector<uint32_t> access_uids;
  uint8_t minimum_level;
  std::array<uint8_t, 7> unknown_1;
};

struct Seed {
  uint32_t time_passed;
  uint8_t item_on_tree;
};

struct Mailbox {
  std::string unknown_1;
  std::string unknown_2;
  std::string unknown_3;
  uint8_t unknown_4;
};

struct Bulletin {
  std::string unknown_1;
  std::string unknown_2;
  std::string unknown_3;
  uint8_t unknown_4;
};

struct Dice {
  uint8_t symbol;
};

struct ChemicalSource {
  uint32_t time_passed;
};

struct AchievementBlock {
  uint32_t unknown_1;
  uint8_t tile_type;
};

struct HearthMonitor {
  uint32_t unknown_1;
  std::string player_name;
};

struct DonationBox {
  std::string unknown_1;
  std::string unknown_2;
  std::string unknown_3;
  uint8_t unknown_4;
};

struct Mannequin {
  std::string text;
  uint8_t unknown_1;
  uint32_t clothing_1;
  uint16_t clothing_2;
  uint16_t clothing_3;
  uint16_t clothing_4;
  uint16_t clothing_5;
  uint16_t clothing_6;
  uint16_t clothing_7;
  uint16_t clothing_8;
  uint16_t clothing_9;
  uint16_t clothing_10;
};

struct BunnyEgg {
  uint32_t egg_placed;
};

struct GamePack {
  uint8_t team;
};

struct GameGenerator {};

struct XenoniteCrystal {
  uint8_t unknown_1;
  uint32_t unknown_2;
};

struct PhoneBooth {
  uint16_t clothing_1;
  uint16_t clothing_2;
  uint16_t clothing_3;
  uint16_t clothing_4;
  uint16_t clothing_5;
  uint16_t clothing_6;
  uint16_t clothing_7;
  uint16_t clothing_8;
  uint16_t clothing_9;
};

struct Crystal {
  std::string unknown_1;
};

struct CrimeInProgress {
  std::string unknown_1;
  uint32_t unknown_2;
  uint8_t unknown_3;
};

struct DisplayBlock {
  uint32_t item_id;
};

struct VendingMachine {
  uint32_t item_id;
  int32_t price;
};

struct GivingTree {
  uint16_t unknown_1;
  uint32_t unknown_2;
};

struct CountryFlag {
  std::string country;
};

struct WeatherMachine {
  uint32_t item_id;
};

struct DataBedrock {
  std::array<uint8_t, 21> unknown_1;
};

enum class TileType {
  Basic,
  Door,
  Sign,
  Lock,
  Seed,
  Mailbox,
  Bulletin,
  Dice,
  ChemicalSource,
  AchievementBlock,
  HearthMonitor,
  DonationBox,
  Mannequin,
  BunnyEgg,
  GamePack,
  GameGenerator,
  XenoniteCrystal,
  PhoneBooth,
  Crystal,
  CrimeInProgress,
  DisplayBlock,
  VendingMachine,
  GivingTree,
  CountryFlag,
  WeatherMachine,
  DataBedrock
};

struct DroppedItem {
  uint16_t id;
  float x;
  float y;
  uint8_t count;
  uint8_t flags;
  uint32_t uid;
};

using TileData =
    std::variant<std::monostate, Door, Sign, Lock, Seed, Mailbox, Bulletin,
                 Dice, ChemicalSource, AchievementBlock, HearthMonitor,
                 DonationBox, Mannequin, BunnyEgg, GamePack, GameGenerator,
                 XenoniteCrystal, PhoneBooth, Crystal, CrimeInProgress,
                 DisplayBlock, VendingMachine, GivingTree, CountryFlag,
                 WeatherMachine, DataBedrock>;

struct Tile {
  uint16_t foreground_item_id;
  uint16_t background_item_id;
  uint16_t parent_block_index;
  uint16_t flags;
  TileType type;
  TileData data;
};

struct Dropped {
  uint32_t items_count;
  uint32_t last_dropped_item_uid;
  std::vector<DroppedItem> items;
};

class World {
public:
  std::string name = "EXIT";
  uint32_t width;
  uint32_t height;
  uint32_t tile_count;
  std::vector<Tile> tiles;
  Dropped dropped;
  uint16_t base_weather;
  uint16_t current_weather;

  void parse(uint8_t *data);

private:
  void get_extra_tile_data(Tile &tile, uint8_t *data, size_t &position,
                           uint8_t extra_tile_type);
};
