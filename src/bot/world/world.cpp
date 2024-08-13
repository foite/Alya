#include "world.hpp"
#include "spdlog/spdlog.h"
#include <cstdint>
#include <cstring>

// There should be better way to write this.
void World::parse(uint8_t *data) {
  this->name = "EXIT";
  this->width = 0;
  this->height = 0;
  this->tile_count = 0;
  this->base_weather = 0;
  this->current_weather = 0;
  this->tiles.clear();

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
    if (tile.foreground_item_id > 14540) {
      spdlog::error("Invalid item id: {} | Position: {} | Index: {}",
                    tile.foreground_item_id, position, i);
    }
    position += sizeof(uint16_t);
    tile.background_item_id =
        *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    tile.parent_block_index =
        *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    tile.flags = *reinterpret_cast<const TileFlags *>(&data[position]);
    position += sizeof(uint16_t);

    if (tile.flags.serialize) {
      uint8_t extra_tile_type = data[position];
      position += sizeof(uint8_t);
      this->get_extra_tile_data(tile, data, position, extra_tile_type);
    }

    if (tile.flags.locked) {
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
  current_weather = *reinterpret_cast<const uint16_t *>(&data[position]);
  position += 6;
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
    position += 4;
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
    if (tile.foreground_item_id != 202 || tile.foreground_item_id != 204 ||
        tile.foreground_item_id != 206 || tile.foreground_item_id != 4994) {

      lock.minimum_level = data[position];
      position += sizeof(uint8_t);
      std::memcpy(lock.unknown_1.data(), &data[position],
                  lock.unknown_1.size());
      position += lock.unknown_1.size();
    }

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
  case 22: { // TileType::Spotlight
    Spotlight spotlight;
    tile.type = TileType::Spotlight;
    tile.data = spotlight;
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
  case 25: { // TileType::FishTankPort
    FishTankPort fish_tank_port;
    fish_tank_port.flags = data[position];
    position += sizeof(uint8_t);
    uint32_t fish_count = *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    for (uint32_t i = 0; i < fish_count; ++i) {
      FishTankPort::info info;
      info.fish_item_id = *reinterpret_cast<const uint32_t *>(&data[position]);
      position += sizeof(uint32_t);
      info.lbs = *reinterpret_cast<const uint32_t *>(&data[position]);
      position += sizeof(uint32_t);
      fish_tank_port.Fishes.push_back(info);
    }
    tile.type = TileType::FishTankPort;
    tile.data = fish_tank_port;
    break;
  }
  case 26: { // TileType::SolarCollector
    SolarCollector solar_collector;
    std::memcpy(solar_collector.unknown_1, &data[position],
                sizeof(solar_collector.unknown_1));
    position += sizeof(solar_collector.unknown_1);
    tile.type = TileType::SolarCollector;
    tile.data = solar_collector;
    break;
  }
  case 27: { // TileType::Forge
    Forge forge;
    forge.temperature = *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    tile.type = TileType::Forge;
    tile.data = forge;
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
  case 30: { // TileType::SteamOrgan
    SteamOrgan steam_organ;
    steam_organ.instrument_type = data[position];
    position += sizeof(uint8_t);
    steam_organ.note = *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    tile.type = TileType::SteamOrgan;
    tile.data = steam_organ;
    break;
  }
  case 31: { // TileType::SilkWorm
    SilkWorm silk_worm;
    silk_worm.type = data[position];
    position += sizeof(uint8_t);
    uint16_t str_len = *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    silk_worm.name.assign(reinterpret_cast<const char *>(&data[position]),
                          str_len);
    position += str_len;
    silk_worm.age = *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    silk_worm.unknown_1 = *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    silk_worm.unknown_2 = *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    silk_worm.can_be_fed = data[position];
    position += sizeof(uint8_t);
    memcpy(&silk_worm.color, &data[position], sizeof(silk_worm.color));
    position += sizeof(silk_worm.color);
    silk_worm.sick_duration =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    tile.type = TileType::SilkWorm;
    tile.data = silk_worm;
    break;
  }
  case 32: { // TileType::SewingMachine
    SewingMachine sewing_machine;
    uint16_t bolt_len = *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    for (uint16_t i = 0; i < bolt_len; ++i) {
      uint32_t bolt_id = *reinterpret_cast<const uint32_t *>(&data[position]);
      position += sizeof(uint32_t);
      sewing_machine.bolt_id_list.push_back(bolt_id);
    }
    tile.type = TileType::SewingMachine;
    tile.data = sewing_machine;
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
  case 34: { // TileType::LobsterTrap
    LobsterTrap lobster_trap;
    tile.type = TileType::LobsterTrap;
    tile.data = lobster_trap;
    break;
  }
  case 35: { // TileType::PaintingEasel
    PaintingEasel painting_easel;
    painting_easel.item_id =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    uint16_t str_len = *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    painting_easel.label.assign(reinterpret_cast<const char *>(&data[position]),
                                str_len);
    position += str_len;
    tile.type = TileType::PaintingEasel;
    tile.data = painting_easel;
    break;
  }
  case 36: { // TileType::PetBattleCage
    PetBattleCage pet_battle_cage;
    uint16_t str_len = *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    pet_battle_cage.label.assign(
        reinterpret_cast<const char *>(&data[position]), str_len);
    position += str_len;
    pet_battle_cage.base_pet =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    pet_battle_cage.combined_pet_11 =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    pet_battle_cage.combined_pet_2 =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    tile.type = TileType::PetBattleCage;
    tile.data = pet_battle_cage;
    break;
  }
  case 37: { // TileType::PetTrainer
    PetTrainer pet_trainer;
    uint16_t str_len = *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    pet_trainer.name.assign(reinterpret_cast<const char *>(&data[position]),
                            str_len);
    position += str_len;
    pet_trainer.pet_total_count =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    pet_trainer.unknown_1 =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    for (uint32_t i = 0; i < pet_trainer.pet_total_count; ++i) {
      uint32_t pet_id = *reinterpret_cast<const uint32_t *>(&data[position]);
      position += sizeof(uint32_t);
      pet_trainer.pets_id.push_back(pet_id);
    }
    tile.type = TileType::PetTrainer;
    tile.data = pet_trainer;
    break;
  }
  case 38: { // TileType::SteamEngine
    SteamEngine steam_engine;
    steam_engine.temperature =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    tile.type = TileType::SteamEngine;
    tile.data = steam_engine;
    break;
  }
  case 39: { // TileType::LockBot
    LockBot lock_bot;
    lock_bot.time_passed = *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    tile.type = TileType::LockBot;
    tile.data = lock_bot;
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
  case 41: { // TileType::SpiritStorageUnit
    SpiritStorageUnit spirit_storage_unit;
    spirit_storage_unit.ghost_jar_count =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    tile.type = TileType::SpiritStorageUnit;
    tile.data = spirit_storage_unit;
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
  case 43: { // TileType::Shelf
    Shelf shelf;
    shelf.top_left_item_id =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    shelf.top_right_item_id =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    shelf.bottom_left_item_id =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    shelf.bottom_right_item_id =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    tile.type = TileType::Shelf;
    tile.data = shelf;
    break;
  }
  case 44: { // TileType::VipEntrance
    VipEntrance vip_entrance;
    vip_entrance.unknown_1 = data[position];
    position += sizeof(uint8_t);
    vip_entrance.owner_uid =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    uint32_t access_count =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    for (uint32_t i = 0; i < access_count; ++i) {
      uint32_t uid = *reinterpret_cast<const uint32_t *>(&data[position]);
      position += sizeof(uint32_t);
      vip_entrance.access_uids.push_back(uid);
    }
    tile.type = TileType::VipEntrance;
    tile.data = vip_entrance;
    break;
  }
  case 45: { // TileType::ChallangeTimer
    ChallangeTimer challange_timer;
    tile.type = TileType::ChallangeTimer;
    tile.data = challange_timer;
    break;
  }
  case 47: { // TileType::FishWallMount
    FishWallMount fish_wall_mount;
    uint16_t str_len = *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    fish_wall_mount.label.assign(
        reinterpret_cast<const char *>(&data[position]), str_len);
    position += str_len;
    fish_wall_mount.item_id =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    fish_wall_mount.lb = data[position];
    position += sizeof(uint8_t);
    tile.type = TileType::FishWallMount;
    tile.data = fish_wall_mount;
    break;
  }
  case 48: { // TileType::Portrait
    Portrait portrait;
    uint16_t str_len = *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    portrait.label.assign(reinterpret_cast<const char *>(&data[position]),
                          str_len);
    position += str_len;
    portrait.unknown_1 = *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    portrait.unknown_2 = *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    portrait.unknown_3 = *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    portrait.unknown_4 = *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    portrait.face = *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    portrait.hat = *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    portrait.hair = *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    portrait.unknown_5 = *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    portrait.unknown_6 = *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    tile.type = TileType::Portrait;
    tile.data = portrait;
    break;
  }
  case 49: { // TileType::GuildWeatherMachine
    GuildWeatherMachine guild_weather_machine;
    guild_weather_machine.unknown_1 =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    guild_weather_machine.gravity = data[position];
    position += sizeof(uint8_t);
    guild_weather_machine.flags = data[position];
    position += sizeof(uint8_t);
    tile.type = TileType::GuildWeatherMachine;
    tile.data = guild_weather_machine;
    break;
  }
  case 50: { // TileType::FossilPrepStation
    FossilPrepStation fossil_prep_station;
    fossil_prep_station.unknown_1 =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    tile.type = TileType::FossilPrepStation;
    tile.data = fossil_prep_station;
    break;
  }
  case 51: { // TileType::DnaExtractor
    DnaExtractor dna_extractor;
    tile.type = TileType::DnaExtractor;
    tile.data = dna_extractor;
    break;
  }
  case 52: { // TileType::Howler
    Howler howler;
    tile.type = TileType::Howler;
    tile.data = howler;
    break;
  }
  case 53: { // TileType::ChemsynthTank
    ChemsynthTank chemsynth_tank;
    chemsynth_tank.current_chem =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    chemsynth_tank.target_chem =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    tile.type = TileType::ChemsynthTank;
    tile.data = chemsynth_tank;
    break;
  }
  case 54: { // TileType::StorageBlock
    StorageBlock storage_block;
    uint32_t item_count = *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    for (uint32_t i = 0; i < item_count; ++i) {
      StorageBlock::ItemInfo item_info;
      item_info.item_id = *reinterpret_cast<const uint32_t *>(&data[position]);
      position += sizeof(uint32_t);
      item_info.amount = *reinterpret_cast<const uint32_t *>(&data[position]);
      position += sizeof(uint32_t);
      storage_block.items.push_back(item_info);
    }
    tile.type = TileType::StorageBlock;
    tile.data = storage_block;
    break;
  }
  case 55: { // TileType::CookingOven
    CookingOven cooking_oven;
    cooking_oven.temperature_level =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    uint32_t ingredient_count =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    for (uint32_t i = 0; i < ingredient_count; ++i) {
      CookingOven::IngredientInfo ingredient_info;
      ingredient_info.item_id =
          *reinterpret_cast<const uint32_t *>(&data[position]);
      position += sizeof(uint32_t);
      ingredient_info.time_added =
          *reinterpret_cast<const uint32_t *>(&data[position]);
      position += sizeof(uint32_t);
      cooking_oven.ingredients.push_back(ingredient_info);
    }
    cooking_oven.unknown_1 =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    cooking_oven.unknown_2 =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    cooking_oven.unknown_3 =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    tile.type = TileType::CookingOven;
    tile.data = cooking_oven;
    break;
  }
  case 56: { // TileType::AudioRack
    AudioRack audio_rack;
    uint32_t note_length = *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    audio_rack.note.assign(reinterpret_cast<const char *>(&data[position]),
                           note_length);
    position += note_length;
    audio_rack.volume = *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    tile.type = TileType::AudioRack;
    tile.data = audio_rack;
    break;
  }
  case 57: { // TileType::GeigerCharger
    GeigerCharger geiger_charger;
    geiger_charger.unknown_1 =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    tile.type = TileType::GeigerCharger;
    tile.data = geiger_charger;
    break;
  }
  case 58: { // TileType::AdventureBegins
    AdventureBegins adventure_begins;
    tile.type = TileType::AdventureBegins;
    tile.data = adventure_begins;
    break;
  }
  case 59: { // TileType::TombRobber
    TombRobber tomb_rober;
    tile.type = TileType::TombRobber;
    tile.data = tomb_rober;
  }
  case 60: { // TileType::BalloonOMatic
    BalloonOMatic balloon_o_matic;
    balloon_o_matic.total_rarity =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    balloon_o_matic.team_type =
        *reinterpret_cast<const uint8_t *>(&data[position]);
    position += sizeof(uint8_t);
    tile.type = TileType::BalloonOMatic;
    tile.data = balloon_o_matic;
    break;
  }
  case 61: { // TileType::TrainingPort
    TrainingPort training_port;
    training_port.fish_lb =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    training_port.fish_status =
        *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    training_port.fish_id =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    training_port.fish_total_exp =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    std::memcpy(training_port.unknown_1, &data[position],
                sizeof(training_port.unknown_1));
    position += sizeof(training_port.unknown_1);
    training_port.fish_level =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    training_port.unknown_2 =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    std::memcpy(training_port.unknown_3, &data[position],
                sizeof(training_port.unknown_3));
    position += sizeof(training_port.unknown_3);
    tile.type = TileType::TrainingPort;
    tile.data = training_port;
    break;
  }
  case 62: { // TileType::ItemSucker
    ItemSucker item_sucker;
    item_sucker.item_id_to_suck =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    item_sucker.item_amount =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    item_sucker.flags = *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    item_sucker.limit = *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    tile.type = TileType::ItemSucker;
    tile.data = item_sucker;
    break;
  }
  case 63: { // TileType::CyBot
    CyBot cy_bot;
    cy_bot.sync_timer = *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    cy_bot.activated = *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    uint32_t command_data_count =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    for (uint32_t i = 0; i < command_data_count; ++i) {
      CyBot::CommandData command_data;
      command_data.command_id =
          *reinterpret_cast<const uint32_t *>(&data[position]);
      position += sizeof(uint32_t);
      command_data.is_command_used =
          *reinterpret_cast<const uint32_t *>(&data[position]);
      position += sizeof(uint32_t);
      std::memcpy(command_data.unknown_1, &data[position],
                  sizeof(command_data.unknown_1));
      position += sizeof(command_data.unknown_1);
      cy_bot.command_datas.push_back(command_data);
    }
    tile.type = TileType::CyBot;
    tile.data = cy_bot;
    break;
  }
  case 65: { // TileType::GuildItem
    GuildItem guild_item;
    std::memcpy(guild_item.unknown_1, &data[position],
                sizeof(guild_item.unknown_1));
    position += sizeof(guild_item.unknown_1);
    tile.type = TileType::GuildItem;
    tile.data = guild_item;
    break;
  }
  case 66: { // TileType::Growscan
    Growscan growscan;
    growscan.unknown_1 = *reinterpret_cast<const uint8_t *>(&data[position]);
    position += sizeof(uint8_t);
    tile.type = TileType::Growscan;
    tile.data = growscan;
    break;
  }
  case 67: { // TileType::ContainmentFieldPowerNode
    ContainmentFieldPowerNode containment_field_power_node;
    containment_field_power_node.ghost_jar_count =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    uint32_t unknown_1_size =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    containment_field_power_node.unknown_1.resize(unknown_1_size);
    std::memcpy(containment_field_power_node.unknown_1.data(), &data[position],
                unknown_1_size * sizeof(uint32_t));
    position += unknown_1_size * sizeof(uint32_t);
    tile.type = TileType::ContainmentFieldPowerNode;
    tile.data = containment_field_power_node;
    break;
  }
  case 68: { // TileType::SpiritBoard
    SpiritBoard spirit_board;
    spirit_board.unknown_1 =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    spirit_board.unknown_2 =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    spirit_board.unknown_3 =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    tile.type = TileType::SpiritBoard;
    tile.data = spirit_board;
    break;
  }
  case 72: { // TileType::StormyCloud
    StormyCloud stormy_cloud;
    stormy_cloud.sting_duration =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    stormy_cloud.is_solid =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    stormy_cloud.non_solid_duration =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    tile.type = TileType::StormyCloud;
    tile.data = stormy_cloud;
    break;
  }
  case 73: { // TileType::TemporaryPlatform
    TemporaryPlatform temporary_platform;
    temporary_platform.unknown_1 =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    tile.type = TileType::TemporaryPlatform;
    tile.data = temporary_platform;
    break;
  }
  case 74: { // TileType::SafeVault
    SafeVault safe_vault;
    tile.type = TileType::SafeVault;
    tile.data = safe_vault;
    break;
  }
  case 75: { // TileType::AngelicCountingCloud
    AngelicCountingCloud angelic_counting_cloud;
    angelic_counting_cloud.is_raffling =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    angelic_counting_cloud.unknown_1 =
        *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);
    angelic_counting_cloud.ascii_code =
        *reinterpret_cast<const uint8_t *>(&data[position]);
    position += sizeof(uint8_t);
    tile.type = TileType::AngelicCountingCloud;
    tile.data = angelic_counting_cloud;
    break;
  }
  case 77: { // TileType::InfinityWeatherMachine
    InfinityWeatherMachine infinity_weather_machine;
    infinity_weather_machine.interval_minutes =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);

    uint32_t weather_machine_list_size =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    infinity_weather_machine.weather_machine_list.resize(
        weather_machine_list_size);
    std::memcpy(infinity_weather_machine.weather_machine_list.data(),
                &data[position], weather_machine_list_size * sizeof(uint32_t));
    position += weather_machine_list_size * sizeof(uint32_t);

    tile.type = TileType::InfinityWeatherMachine;
    tile.data = infinity_weather_machine;
    break;
  }
  case 79: { // TileType::PineappleGuzzler
    PineappleGuzzler pineapple_guzzler;
    tile.type = TileType::PineappleGuzzler;
    tile.data = pineapple_guzzler;
    break;
  }
  case 80: { // TileType::KrakenGalaticBlock
    KrakenGalaticBlock kraken_galatic_block;
    kraken_galatic_block.pattern_index =
        *reinterpret_cast<const uint8_t *>(&data[position]);
    position += sizeof(uint8_t);
    kraken_galatic_block.unknown_1 =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    kraken_galatic_block.r =
        *reinterpret_cast<const uint8_t *>(&data[position]);
    position += sizeof(uint8_t);
    kraken_galatic_block.g =
        *reinterpret_cast<const uint8_t *>(&data[position]);
    position += sizeof(uint8_t);
    kraken_galatic_block.b =
        *reinterpret_cast<const uint8_t *>(&data[position]);
    position += sizeof(uint8_t);
    tile.type = TileType::KrakenGalaticBlock;
    tile.data = kraken_galatic_block;
    break;
  }
  case 81: { // TileType::FriendsEntrance
    FriendsEntrance friends_entrance;
    friends_entrance.owner_user_id =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    friends_entrance.unknown_1 =
        *reinterpret_cast<const uint16_t *>(&data[position]);
    position += sizeof(uint16_t);

    uint32_t allowed_user_list_size =
        *reinterpret_cast<const uint32_t *>(&data[position]);
    position += sizeof(uint32_t);
    friends_entrance.allowed_user_list.resize(allowed_user_list_size);
    std::memcpy(friends_entrance.allowed_user_list.data(), &data[position],
                allowed_user_list_size * sizeof(uint32_t));
    position += allowed_user_list_size * sizeof(uint32_t);

    tile.type = TileType::FriendsEntrance;
    tile.data = friends_entrance;
    break;
  }
  default:
    tile.type = TileType::Basic;
  }
}