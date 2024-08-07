#pragma once
#include <cstdint>
#include <string>
#include <vector>

struct Item {
  uint32_t id;
  uint16_t flags;
  char type;
  char material;
  std::string name;
  std::string texture_name;
  uint32_t texture_hash;
  char visual_effect;
  uint32_t cooking_ingredient_type;
  char texture_x;
  char texture_y;
  char render_type;
  char is_stripey_wallpaper;
  char collision_type;
  char block_health;
  uint32_t drop_chance;
  char clothing_type;
  uint16_t rarity;
  char max_items;
  std::string extra_file_name;
  uint32_t extra_file_hash;
  uint32_t audio_volume;
  std::string pet_name;
  std::string pet_prefix;
  std::string pet_suffix;
  std::string pet_ability;
  char seed_base_sprite;
  char seed_base_overlay;
  char tree_base_sprite;
  char tree_base_overlay;
  uint32_t base_color;
  uint32_t overlay_color;
  uint32_t ingredient;
  uint32_t grow_time;
  uint16_t is_rayman;
  std::string extra_options;
  std::string texture_path_2;
  std::string extra_options_2;
  std::string punch_option;
};

struct ItemDatabase {
  uint16_t version;
  uint32_t item_count;
  std::vector<Item> items;
};

std::string decrypt_string(FILE *file, uint32_t id);
std::string read_string(FILE *file);
ItemDatabase *parse_from_file(const char *filename);