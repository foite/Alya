#pragma once
#include <array>
#include <cstdint>
#include <string>
#include <sys/types.h>
#include <variant>
#include <vector>

struct Door {
  std::string text;
  uint8_t unknown_1;
};

struct Sign {
  std::string text;
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

struct Spotlight {};

struct FishTankPort {
  uint8_t flags;
  struct info {
    uint32_t fish_item_id, lbs;
  };
  std::vector<info> Fishes;
};

struct SolarCollector {
  uint8_t unknown_1[5];
};

struct Forge {
  uint32_t temperature;
};

struct SteamOrgan {
  uint8_t instrument_type;
  uint32_t note;
};

struct SilkWorm {
  uint8_t type;
  std::string name;
  uint32_t age;
  uint32_t unknown_1;
  uint32_t unknown_2;
  uint8_t can_be_fed;
  uint32_t food_saturation;
  uint32_t water_saturation;
  struct {
    uint8_t A;
    uint8_t R;
    uint8_t G;
    uint8_t B;
  } color;
  uint32_t sick_duration;
};

struct SewingMachine {
  std::vector<uint32_t> bolt_id_list;
};

struct LobsterTrap {};

struct PaintingEasel {
  int32_t item_id;
  std::string label;
};

struct PetBattleCage {
  std::string label;
  uint32_t base_pet;
  uint32_t combined_pet_11;
  uint32_t combined_pet_2;
};

struct PetTrainer {
  std::string name;
  uint32_t pet_total_count;
  uint32_t unknown_1;
  std::vector<uint32_t> pets_id;
};

struct SteamEngine {
  uint32_t temperature;
};

struct LockBot {
  uint32_t time_passed;
};

struct SpiritStorageUnit {
  uint32_t ghost_jar_count;
};

struct Shelf {
  uint32_t top_left_item_id;
  uint32_t top_right_item_id;
  uint32_t bottom_left_item_id;
  uint32_t bottom_right_item_id;
};

struct VipEntrance {
  uint8_t unknown_1;
  uint32_t owner_uid;
  std::vector<uint32_t> access_uids;
};

struct ChallangeTimer {};

struct FishWallMount {
  std::string label;
  uint32_t item_id;
  uint8_t lb;
};

struct Portrait {
  std::string label;
  uint32_t unknown_1, unknown_2, unknown_3, unknown_4;
  uint32_t face, hat, hair;
  uint16_t unknown_5, unknown_6;
};

struct GuildWeatherMachine {
  uint32_t unknown_1;
  uint32_t gravity;
  uint8_t flags;
};

struct FossilPrepStation {
  uint32_t unknown_1;
};

struct DnaExtractor {};

struct Howler {};

struct ChemsynthTank {
  uint32_t current_chem;
  uint32_t target_chem;
};

struct StorageBlock {
  struct ItemInfo {
    uint32_t item_id, amount;
  };
  std::vector<ItemInfo> items;
};

struct CookingOven {
  struct IngredientInfo {
    uint32_t item_id, time_added;
  };
  uint32_t temperature_level;
  std::vector<IngredientInfo> ingredients;
  uint32_t unknown_1, unknown_2, unknown_3;
};

struct AudioRack {
  std::string note;
  uint32_t volume;
};

struct GeigerCharger {
  uint32_t unknown_1;
};

struct AdventureBegins {};

struct TombRobber {};

struct BalloonOMatic {
  uint32_t total_rarity;
  uint8_t team_type;
};

struct TrainingPort {
  uint32_t fish_lb;
  uint16_t fish_status;
  uint32_t fish_id;
  uint32_t fish_total_exp;
  uint8_t unknown_1[8];
  uint32_t fish_level;
  uint32_t unknown_2;
  uint8_t unknown_3[5];
};

struct ItemSucker {
  uint32_t item_id_to_suck;
  uint32_t item_amount;
  uint16_t flags;
  uint32_t limit;
};

struct CyBot {
  struct CommandData {
    uint32_t command_id;
    uint32_t is_command_used;
    uint8_t unknown_1[7];
  };
  uint32_t sync_timer;
  uint32_t activated;
  std::vector<CommandData> command_datas;
};

struct GuildItem {
  uint8_t unknown_1[17];
};

struct Growscan {
  uint8_t unknown_1;
};

struct ContainmentFieldPowerNode {
  uint32_t ghost_jar_count;
  std::vector<uint32_t> unknown_1;
};

struct SpiritBoard {
  uint32_t unknown_1;
  uint32_t unknown_2;
  uint32_t unknown_3;
};

struct StormyCloud {
  uint32_t sting_duration;
  uint32_t is_solid;
  uint32_t non_solid_duration;
};

struct TemporaryPlatform {
  uint32_t unknown_1;
};

struct SafeVault {};

struct AngelicCountingCloud {
  uint32_t is_raffling;
  uint16_t unknown_1;
  uint8_t ascii_code;
};

struct InfinityWeatherMachine {
  uint32_t interval_minutes;
  std::vector<uint32_t> weather_machine_list;
};

struct PineappleGuzzler {};

struct KrakenGalaticBlock {
  uint8_t pattern_index;
  uint32_t unknown_1;
  uint8_t r, g, b;
};

struct FriendsEntrance {
  uint32_t owner_user_id;
  uint16_t unknown_1;
  std::vector<uint32_t> allowed_user_list;
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
  DataBedrock,
  Spotlight,
  FishTankPort,
  SolarCollector,
  Forge,
  SteamOrgan,
  SilkWorm,
  SewingMachine,
  LobsterTrap,
  PaintingEasel,
  PetBattleCage,
  PetTrainer,
  SteamEngine,
  LockBot,
  SpiritStorageUnit,
  Shelf,
  VipEntrance,
  ChallangeTimer,
  FishWallMount,
  Portrait,
  GuildWeatherMachine,
  FossilPrepStation,
  DnaExtractor,
  Howler,
  ChemsynthTank,
  StorageBlock,
  CookingOven,
  AudioRack,
  GeigerCharger,
  AdventureBegins,
  TombRobber,
  BalloonOMatic,
  TrainingPort,
  ItemSucker,
  CyBot,
  GuildItem,
  Growscan,
  ContainmentFieldPowerNode,
  SpiritBoard,
  StormyCloud,
  TemporaryPlatform,
  SafeVault,
  AngelicCountingCloud,
  InfinityWeatherMachine,
  PineappleGuzzler,
  KrakenGalaticBlock,
  FriendsEntrance
};

struct DroppedItem {
  uint16_t id = 0;
  float x = 0;
  float y = 0;
  uint8_t count = 0;
  uint8_t flags = 0;
  uint32_t uid = 0;
};

using TileData = std::variant<
    std::monostate, Door, Sign, Lock, Seed, Mailbox, Bulletin, Dice,
    ChemicalSource, AchievementBlock, HearthMonitor, DonationBox, Mannequin,
    BunnyEgg, GamePack, GameGenerator, XenoniteCrystal, PhoneBooth, Crystal,
    CrimeInProgress, DisplayBlock, VendingMachine, GivingTree, CountryFlag,
    WeatherMachine, DataBedrock, Spotlight, FishTankPort, SolarCollector, Forge,
    SteamOrgan, SilkWorm, SewingMachine, LobsterTrap, PaintingEasel,
    PetBattleCage, PetTrainer, SteamEngine, LockBot, SpiritStorageUnit, Shelf,
    VipEntrance, ChallangeTimer, FishWallMount, Portrait, GuildWeatherMachine,
    FossilPrepStation, DnaExtractor, Howler, ChemsynthTank, StorageBlock,
    CookingOven, AudioRack, GeigerCharger, AdventureBegins, TombRobber,
    BalloonOMatic, TrainingPort, ItemSucker, CyBot, GuildItem, Growscan,
    ContainmentFieldPowerNode, SpiritBoard, StormyCloud, TemporaryPlatform,
    SafeVault, AngelicCountingCloud, InfinityWeatherMachine, PineappleGuzzler,
    KrakenGalaticBlock, FriendsEntrance>;

struct TileFlags {
  uint16_t serialize : 1;
  uint16_t locked : 1;
  uint16_t unknown3 : 1;
  uint16_t unknown4 : 1;
  uint16_t enable_growth : 1;
  uint16_t rotation : 1;
  uint16_t enabled : 1;
  uint16_t public_access : 1;
  uint16_t unknown5 : 1;
  uint16_t unknown6 : 1;
  uint16_t water : 1;
  uint16_t glue : 1;
  uint16_t fire : 1;
  uint16_t red : 1;
  uint16_t green : 1;
  uint16_t blue : 1;
};

struct Tile {
  uint16_t foreground_item_id = 0;
  uint16_t background_item_id = 0;
  uint16_t parent_block_index = 0;
  TileFlags flags;
  TileType type = TileType::Basic;
  TileData data;
};

struct Dropped {
  uint32_t items_count = 0;
  uint32_t last_dropped_item_uid = 0;
  std::vector<DroppedItem> items;
};

class World {
public:
  std::string name = "EXIT";
  uint32_t width = 0;
  uint32_t height = 0;
  uint32_t tile_count = 0;
  std::vector<Tile> tiles;
  Dropped dropped;
  uint16_t base_weather = 0;
  uint16_t current_weather = 0;

  void parse(uint8_t *data);

private:
  void get_extra_tile_data(Tile &tile, uint8_t *data, size_t &position,
                           uint8_t extra_tile_type);
};
