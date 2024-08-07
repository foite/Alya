#include "item.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

const char *CIPHER = "PBG892FXX982ABC*";

std::string decrypt_string(FILE *file, uint32_t item_id) {
  uint16_t len = 0;
  fread(&len, sizeof(uint16_t), 1, file);
  std::string str(len, '\0');

  for (uint16_t i = 0; i < len; i++) {
    int secret_char_pos = (i + item_id) % strlen(CIPHER);
    char secret_char = CIPHER[secret_char_pos];
    char input_char;
    fread(&input_char, sizeof(char), 1, file);
    str[i] = input_char ^ secret_char;
  }
  return str;
}

std::string read_string(FILE *file) {
  uint16_t len = 0;
  fread(&len, sizeof(uint16_t), 1, file);
  std::string str(len, '\0');
  fread(&str[0], sizeof(char), len, file);
  return str;
}

ItemDatabase *parse_from_file(const char *filename) {
  auto db = new ItemDatabase();
  FILE *file = fopen(filename, "r");
  if (file == nullptr) {
    perror("Error opening file\n");
    delete db;
    return nullptr;
  }

  fread(&db->version, sizeof(uint16_t), 1, file);
  fread(&db->item_count, sizeof(uint32_t), 1, file);

  db->items.resize(db->item_count);

  for (uint32_t i = 0; i < db->item_count; i++) {
    fread(&db->items[i].id, sizeof(uint32_t), 1, file);
    if (i != db->items[i].id) {
      perror("Item ID mismatch\n");
      fclose(file);
      delete db;
      return nullptr;
    }

    fread(&db->items[i].flags, sizeof(uint16_t), 1, file);
    fread(&db->items[i].type, sizeof(char), 1, file);
    fread(&db->items[i].material, sizeof(char), 1, file);
    db->items[i].name = decrypt_string(file, db->items[i].id);
    db->items[i].texture_name = read_string(file);
    fread(&db->items[i].texture_hash, sizeof(uint32_t), 1, file);
    fread(&db->items[i].visual_effect, sizeof(char), 1, file);
    fread(&db->items[i].cooking_ingredient_type, sizeof(uint32_t), 1, file);
    fread(&db->items[i].texture_x, sizeof(char), 1, file);
    fread(&db->items[i].texture_y, sizeof(char), 1, file);
    fread(&db->items[i].render_type, sizeof(char), 1, file);
    fread(&db->items[i].is_stripey_wallpaper, sizeof(char), 1, file);
    fread(&db->items[i].collision_type, sizeof(char), 1, file);
    fread(&db->items[i].block_health, sizeof(char), 1, file);
    fread(&db->items[i].drop_chance, sizeof(uint32_t), 1, file);
    fread(&db->items[i].clothing_type, sizeof(char), 1, file);
    fread(&db->items[i].rarity, sizeof(uint16_t), 1, file);
    fread(&db->items[i].max_items, sizeof(char), 1, file);
    db->items[i].extra_file_name = read_string(file);
    fread(&db->items[i].extra_file_hash, sizeof(uint32_t), 1, file);
    fread(&db->items[i].audio_volume, sizeof(uint32_t), 1, file);
    db->items[i].pet_name = read_string(file);
    db->items[i].pet_prefix = read_string(file);
    db->items[i].pet_suffix = read_string(file);
    db->items[i].pet_ability = read_string(file);
    fread(&db->items[i].seed_base_sprite, sizeof(char), 1, file);
    fread(&db->items[i].seed_base_overlay, sizeof(char), 1, file);
    fread(&db->items[i].tree_base_sprite, sizeof(char), 1, file);
    fread(&db->items[i].tree_base_overlay, sizeof(char), 1, file);
    fread(&db->items[i].base_color, sizeof(uint32_t), 1, file);
    fread(&db->items[i].overlay_color, sizeof(uint32_t), 1, file);
    fread(&db->items[i].ingredient, sizeof(uint32_t), 1, file);
    fread(&db->items[i].grow_time, sizeof(uint32_t), 1, file);
    fseek(file, sizeof(uint16_t), SEEK_CUR);
    fread(&db->items[i].is_rayman, sizeof(uint16_t), 1, file);
    db->items[i].extra_options = read_string(file);
    db->items[i].texture_path_2 = read_string(file);
    db->items[i].extra_options_2 = read_string(file);
    fseek(file, 80, SEEK_CUR);

    if (db->version >= 11) {
      db->items[i].punch_option = read_string(file);
    }
    if (db->version >= 12) {
      fseek(file, 13, SEEK_CUR);
    }
    if (db->version >= 13) {
      fseek(file, 4, SEEK_CUR);
    }
    if (db->version >= 14) {
      fseek(file, 4, SEEK_CUR);
    }
    if (db->version >= 15) {
      fseek(file, 25, SEEK_CUR);
      read_string(file);
    }
    if (db->version >= 16) {
      read_string(file);
    }
    if (db->version >= 17) {
      fseek(file, 4, SEEK_CUR);
    }
    if (db->version >= 18) {
      fseek(file, 4, SEEK_CUR);
    }
  }
  fclose(file);
  return db;
}