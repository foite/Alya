#include "world.hpp"
#include <cstring>

void World::parse(uint8_t *data) {
  size_t position = 0;
  position += 6;

  uint16_t str_len = *reinterpret_cast<const uint16_t *>(&data[position]);
  position += sizeof(uint16_t);

  this->name.assign(reinterpret_cast<const char *>(&data[position]), str_len);
  position += str_len;

  this->width = *reinterpret_cast<const uint32_t *>(&data[position]);
  position += sizeof(uint32_t);
  this->height = *reinterpret_cast<const uint32_t *>(&data[position]);
  position += sizeof(uint32_t);
  this->tile_count = *reinterpret_cast<const uint32_t *>(&data[position]);
  position += sizeof(uint32_t);

  position += 5;

  for (uint32_t i = 0; i < tile_count; ++i) {
    Tile tile;
    tile.foreground_item_id =
        *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    tile.background_item_id =
        *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    tile.parent_block_index =
        *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    tile.flags = *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);

    if ((tile.flags & 0x1) != 0) {
      uint8_t extra_tile_type = data[position];
      position += sizeof(uint8_t);
      this->get_extra_tile_data(tile, data, position, extra_tile_type);
    }

    if ((tile.flags & 0x2) != 0) {
      position += sizeof(uint16_t);
    }

    this->tiles.push_back(tile);
  }

  position += 12;

  this->dropped.items_count =
      *reinterpret_cast<const uint32_t *>(&data[position]);
  position += sizeof(uint32_t);

  this->dropped.last_dropped_item_uid =
      *reinterpret_cast<const uint32_t *>(&data[position]);
  position += sizeof(uint32_t);

  for (uint32_t i = 0; i < dropped.items_count; ++i) {
    DroppedItem item;
    item.id = *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    item.x = *reinterpret_cast<const float *>(&data[position]);
    position += sizeof(float);
    item.y = *reinterpret_cast<const float *>(&data[position]);
    position += sizeof(float);
    item.count = data[position];
    position += sizeof(uint8_t);
    item.flags = data[position];
    position += sizeof(uint8_t);
    item.uid = *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    dropped.items.push_back(item);
  }

  base_weather = *reinterpret_cast<const uint16_t *>(&data[position]);
  position += sizeof(uint16_t);

  position += sizeof(uint16_t); // unknown

  current_weather = *reinterpret_cast<const uint16_t *>(&data[position]);
  position += sizeof(uint16_t);
}

void World::get_extra_tile_data(Tile &tile, uint8_t *data, size_t &position,
                                uint8_t extra_tile_type) {
  switch (extra_tile_type) {
  case 1: { // TileType::Door
    Door door;
    uint16_t str_len = *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    door.text.assign(reinterpret_cast<const char *>(&data[position]), str_len);
    position += str_len;
    door.unknown_1 = data[position];
    position += sizeof(uint8_t);
    tile.type = TileType::Door;
    tile.data = door;
    break;
  }
  case 2: { // TileType::Sign
    Sign sign;
    uint16_t str_len = *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    sign.text.assign(reinterpret_cast<const char *>(&data[position]), str_len);
    position += str_len;
    sign.unknown_1 = data[position];
    position += sizeof(uint8_t);
    tile.type = TileType::Sign;
    tile.data = sign;
    break;
  }
  case 3: { // TileType::Lock
    Lock lock;
    lock.settings = data[position];
    position += sizeof(uint8_t);
    lock.owner_uid = *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    lock.access_count = *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    for (uint32_t i = 0; i < lock.access_count; ++i) {
      uint32_t uid = *reinterpret_cast<const uint32_t *>(&data[position]);
      position += sizeof(uint32_t);
      lock.access_uids.push_back(uid);
    }
    lock.minimum_level = data[position];
    position += sizeof(uint8_t);
    std::memcpy(lock.unknown_1.data(), &data[position], lock.unknown_1.size());
    position += lock.unknown_1.size();

    if (tile.foreground_item_id == 5814) {
      position += 16; // Skip 16 bytes
    }

    tile.type = TileType::Lock;
    tile.data = lock;
    break;
  }
  case 4: { // TileType::Seed
    Seed seed;
    seed.time_passed = *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    seed.item_on_tree = data[position];
    position += sizeof(uint8_t);
    tile.type = TileType::Seed;
    tile.data = seed;
    break;
  }
  case 6: { // TileType::Mailbox
    Mailbox mailbox;
    uint16_t str_len = *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    mailbox.unknown_1.assign(reinterpret_cast<const char *>(&data[position]),
                             str_len);
    position += str_len;
    str_len = *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    mailbox.unknown_2.assign(reinterpret_cast<const char *>(&data[position]),
                             str_len);
    position += str_len;
    str_len = *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    mailbox.unknown_3.assign(reinterpret_cast<const char *>(&data[position]),
                             str_len);
    position += str_len;
    mailbox.unknown_4 = data[position];
    position += sizeof(uint8_t);
    tile.type = TileType::Mailbox;
    tile.data = mailbox;
    break;
  }
  case 7: { // TileType::Bulletin
    Bulletin bulletin;
    uint16_t str_len = *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    bulletin.unknown_1.assign(reinterpret_cast<const char *>(&data[position]),
                              str_len);
    position += str_len;
    str_len = *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    bulletin.unknown_2.assign(reinterpret_cast<const char *>(&data[position]),
                              str_len);
    position += str_len;
    str_len = *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    bulletin.unknown_3.assign(reinterpret_cast<const char *>(&data[position]),
                              str_len);
    position += str_len;
    bulletin.unknown_4 = data[position];
    position += sizeof(uint8_t);
    tile.type = TileType::Bulletin;
    tile.data = bulletin;
    break;
  }
  case 8: { // TileType::Dice
    Dice dice;
    dice.symbol = data[position];
    position += sizeof(uint8_t);
    tile.type = TileType::Dice;
    tile.data = dice;
    break;
  }
  case 9: { // TileType::ChemicalSource
    ChemicalSource chemical_source;
    chemical_source.time_passed =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    tile.type = TileType::ChemicalSource;
    tile.data = chemical_source;
    break;
  }
  case 10: { // TileType::AchievementBlock
    AchievementBlock achievement_block;
    achievement_block.unknown_1 =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    achievement_block.tile_type = data[position];
    position += sizeof(uint8_t);
    tile.type = TileType::AchievementBlock;
    tile.data = achievement_block;
    break;
  }
  case 11: { // TileType::HearthMonitor
    HearthMonitor hearth_monitor;
    hearth_monitor.unknown_1 =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    uint16_t str_len = *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    hearth_monitor.player_name.assign(
        reinterpret_cast<const char *>(&data[position]), str_len);
    position += str_len;
    tile.type = TileType::HearthMonitor;
    tile.data = hearth_monitor;
    break;
  }
  case 12: { // TileType::DonationBox
    DonationBox donation_box;
    uint16_t str_len = *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    donation_box.unknown_1.assign(
        reinterpret_cast<const char *>(&data[position]), str_len);
    position += str_len;
    str_len = *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    donation_box.unknown_2.assign(
        reinterpret_cast<const char *>(&data[position]), str_len);
    position += str_len;
    str_len = *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    donation_box.unknown_3.assign(
        reinterpret_cast<const char *>(&data[position]), str_len);
    position += str_len;
    donation_box.unknown_4 = data[position];
    position += sizeof(uint8_t);
    tile.type = TileType::DonationBox;
    tile.data = donation_box;
    break;
  }
  case 14: { // TileType::Mannequin
    Mannequin mannequin;
    uint16_t str_len = *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    mannequin.text.assign(reinterpret_cast<const char *>(&data[position]),
                          str_len);
    position += str_len;
    mannequin.unknown_1 = data[position];
    position += sizeof(uint8_t);
    mannequin.clothing_1 = *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    mannequin.clothing_2 = *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    mannequin.clothing_3 = *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    mannequin.clothing_4 = *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    mannequin.clothing_5 = *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    mannequin.clothing_6 = *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    mannequin.clothing_7 = *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    mannequin.clothing_8 = *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    mannequin.clothing_9 = *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    mannequin.clothing_10 =
        *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    tile.type = TileType::Mannequin;
    tile.data = mannequin;
    break;
  }
  case 15: { // TileType::BunnyEgg
    BunnyEgg bunny_egg;
    bunny_egg.egg_placed = *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    tile.type = TileType::BunnyEgg;
    tile.data = bunny_egg;
    break;
  }
  case 16: { // TileType::GamePack
    GamePack game_pack;
    game_pack.team = data[position];
    position += sizeof(uint8_t);
    tile.type = TileType::GamePack;
    tile.data = game_pack;
    break;
  }
  case 17: { // TileType::GameGenerator
    GameGenerator game_generator;
    tile.type = TileType::GameGenerator;
    tile.data = game_generator;
    break;
  }
  case 18: { // TileType::XenoniteCrystal
    XenoniteCrystal xenonite_crystal;
    xenonite_crystal.unknown_1 = data[position];
    position += sizeof(uint8_t);
    xenonite_crystal.unknown_2 =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    tile.type = TileType::XenoniteCrystal;
    tile.data = xenonite_crystal;
    break;
  }
  case 19: { // TileType::PhoneBooth
    PhoneBooth phone_booth;
    phone_booth.clothing_1 =
        *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    phone_booth.clothing_2 =
        *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    phone_booth.clothing_3 =
        *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    phone_booth.clothing_4 =
        *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    phone_booth.clothing_5 =
        *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    phone_booth.clothing_6 =
        *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    phone_booth.clothing_7 =
        *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    phone_booth.clothing_8 =
        *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    phone_booth.clothing_9 =
        *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    tile.type = TileType::PhoneBooth;
    tile.data = phone_booth;
    break;
  }
  case 20: { // TileType::Crystal
    Crystal crystal;
    uint16_t str_len = *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    crystal.unknown_1.assign(reinterpret_cast<const char *>(&data[position]),
                             str_len);
    position += str_len;
    tile.type = TileType::Crystal;
    tile.data = crystal;
    break;
  }
  case 21: { // TileType::CrimeInProgress
    CrimeInProgress crime_in_progress;
    uint16_t str_len = *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    crime_in_progress.unknown_1.assign(
        reinterpret_cast<const char *>(&data[position]), str_len);
    position += str_len;
    crime_in_progress.unknown_2 =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    crime_in_progress.unknown_3 = data[position];
    position += sizeof(uint8_t);
    tile.type = TileType::CrimeInProgress;
    tile.data = crime_in_progress;
    break;
  }
  case 23: { // TileType::DisplayBlock
    DisplayBlock display_block;
    display_block.item_id =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    tile.type = TileType::DisplayBlock;
    tile.data = display_block;
    break;
  }
  case 24: { // TileType::VendingMachine
    VendingMachine vending_machine;
    vending_machine.item_id =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    vending_machine.price = *reinterpret_cast<const int32_t *>(&data[position]);
    position += sizeof(int32_t);
    tile.type = TileType::VendingMachine;
    tile.data = vending_machine;
    break;
  }
  case 28: { // TileType::GivingTree
    GivingTree giving_tree;
    giving_tree.unknown_1 =
        *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    giving_tree.unknown_2 =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    tile.type = TileType::GivingTree;
    tile.data = giving_tree;
    break;
  }
  case 33: { // TileType::CountryFlag
    CountryFlag country_flag;
    uint16_t str_len = *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    country_flag.country.assign(reinterpret_cast<const char *>(&data[position]),
                                str_len);
    position += str_len;
    tile.type = TileType::CountryFlag;
    tile.data = country_flag;
    break;
  }
  case 40: { // TileType::WeatherMachine
    WeatherMachine weather_machine;
    weather_machine.item_id =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    tile.type = TileType::WeatherMachine;
    tile.data = weather_machine;
    break;
  }
  case 42: { // TileType::DataBedrock
    DataBedrock data_bedrock;
    std::memcpy(data_bedrock.unknown_1.data(), &data[position],
                data_bedrock.unknown_1.size());
    position += data_bedrock.unknown_1.size();
    tile.type = TileType::DataBedrock;
    tile.data = data_bedrock;
    break;
  }
  default:
    tile.type = TileType::Basic;
  }
}