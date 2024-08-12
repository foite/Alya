#include "item.hpp"
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

const char *CIPHER = "PBG892FXX982ABC*";

std::string decrypt_string(std::ifstream &file, uint32_t item_id) {
  uint16_t len = 0;
  file.read(reinterpret_cast<char *>(&len), sizeof(uint16_t));
  std::string str(len, '\0');

  for (uint16_t i = 0; i < len; i++) {
    int secret_char_pos = (i + item_id) % strlen(CIPHER);
    char secret_char = CIPHER[secret_char_pos];
    char input_char;
    file.read(&input_char, sizeof(char));
    str[i] = input_char ^ secret_char;
  }
  return str;
}

std::string read_string(std::ifstream &file) {
  uint16_t len = 0;
  file.read(reinterpret_cast<char *>(&len), sizeof(uint16_t));
  std::string str(len, '\0');
  file.read(&str[0], len);
  return str;
}

std::shared_ptr<ItemDatabase> parse_from_file(const char *filename) {
  auto db = std::make_shared<ItemDatabase>();
  std::ifstream file(filename, std::ios::binary);
  if (!file) {
    std::cerr << "Error opening file\n";
    return nullptr;
  }

  file.read(reinterpret_cast<char *>(&db->version), sizeof(uint16_t));
  file.read(reinterpret_cast<char *>(&db->item_count), sizeof(uint32_t));

  db->items.reserve(db->item_count);

  for (uint32_t i = 0; i < db->item_count; i++) {
    Item item;
    file.read(reinterpret_cast<char *>(&item.id), sizeof(uint32_t));
    if (i != item.id) {
      std::cerr << "Item ID mismatch\n";
      return nullptr;
    }

    file.read(reinterpret_cast<char *>(&item.flags), sizeof(uint16_t));
    file.read(&item.type, sizeof(char));
    file.read(&item.material, sizeof(char));
    item.name = decrypt_string(file, item.id);
    item.texture_name = read_string(file);
    file.read(reinterpret_cast<char *>(&item.texture_hash), sizeof(uint32_t));
    file.read(&item.visual_effect, sizeof(char));
    file.read(reinterpret_cast<char *>(&item.cooking_ingredient_type),
              sizeof(uint32_t));
    file.read(&item.texture_x, sizeof(char));
    file.read(&item.texture_y, sizeof(char));
    file.read(&item.render_type, sizeof(char));
    file.read(&item.is_stripey_wallpaper, sizeof(char));
    file.read(&item.collision_type, sizeof(char));
    file.read(&item.block_health, sizeof(char));
    file.read(reinterpret_cast<char *>(&item.drop_chance), sizeof(uint32_t));
    file.read(&item.clothing_type, sizeof(char));
    file.read(reinterpret_cast<char *>(&item.rarity), sizeof(uint16_t));
    file.read(&item.max_items, sizeof(char));
    item.extra_file_name = read_string(file);
    file.read(reinterpret_cast<char *>(&item.extra_file_hash),
              sizeof(uint32_t));
    file.read(reinterpret_cast<char *>(&item.audio_volume), sizeof(uint32_t));
    item.pet_name = read_string(file);
    item.pet_prefix = read_string(file);
    item.pet_suffix = read_string(file);
    item.pet_ability = read_string(file);
    file.read(&item.seed_base_sprite, sizeof(char));
    file.read(&item.seed_base_overlay, sizeof(char));
    file.read(&item.tree_base_sprite, sizeof(char));
    file.read(&item.tree_base_overlay, sizeof(char));
    file.read(reinterpret_cast<char *>(&item.base_color), sizeof(uint32_t));
    file.read(reinterpret_cast<char *>(&item.overlay_color), sizeof(uint32_t));
    file.read(reinterpret_cast<char *>(&item.ingredient), sizeof(uint32_t));
    file.read(reinterpret_cast<char *>(&item.grow_time), sizeof(uint32_t));
    file.seekg(sizeof(uint16_t), std::ios::cur);
    file.read(reinterpret_cast<char *>(&item.is_rayman), sizeof(uint16_t));
    item.extra_options = read_string(file);
    item.texture_path_2 = read_string(file);
    item.extra_options_2 = read_string(file);
    file.seekg(80, std::ios::cur);

    if (db->version >= 11) {
      item.punch_option = read_string(file);
    }
    if (db->version >= 12) {
      file.seekg(13, std::ios::cur);
    }
    if (db->version >= 13) {
      file.seekg(4, std::ios::cur);
    }
    if (db->version >= 14) {
      file.seekg(4, std::ios::cur);
    }
    if (db->version >= 15) {
      file.seekg(25, std::ios::cur);
      read_string(file);
    }
    if (db->version >= 16) {
      read_string(file);
    }
    if (db->version >= 17) {
      file.seekg(4, std::ios::cur);
    }
    if (db->version >= 18) {
      file.seekg(4, std::ios::cur);
    }

    db->items[item.id] = item;
  }
  return db;
}