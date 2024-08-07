#include "variant_handler.hpp"
#include "types/e_packet_type.hpp"
#include "utils/textparse.hpp"
#include "utils/variant.hpp"
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

void onSendToServer(Bot *bot, const utils::VariantList &variant) {
  int32_t port = variant.get(1)->as_int32();
  int32_t token = variant.get(2)->as_int32();
  int32_t user_id = variant.get(3)->as_int32();
  std::string server_data = variant.get(4)->as_string();
  std::vector<std::string> parsed_server_data =
      utils::TextParse::parse_and_store_as_vec(server_data);

  bot->state.is_redirect = true;
  bot->server.ip = parsed_server_data.at(0);
  bot->server.port = port;
  bot->info.login_info.token = std::to_string(token);
  bot->info.login_info.user = std::to_string(user_id);
  bot->info.login_info.door_id = parsed_server_data.at(1);
  bot->info.login_info.uuid = parsed_server_data.at(2);

  bot->disconnect();
}

void onSuperMainStartAcceptLogonHrdxs47254722215a(Bot *bot,
                                                  const utils::VariantList &) {
  bot->send_packet(types::EPacketType::NetMessageGenericText,
                   "action|enter_game\n");
  bot->state.is_redirect = false;
}

void onCountryState(Bot *bot, const utils::VariantList &) {
  bot->send_packet(types::EPacketType::NetMessageGenericText,
                   "action|getDRAnimations\n");
  bot->send_packet(types::EPacketType::NetMessageGenericText,
                   "action|getDRAnimations\n");
}

void onDialogRequest(Bot *bot, const utils::VariantList &variant) {
  std::string message = variant.get(1)->as_string();
  if (message.find("Gazette") != std::string::npos) {
    bot->send_packet(
        types::EPacketType::NetMessageGenericText,
        "action|dialog_return\ndialog_name|gazette\nbuttonClicked|banner\n");
  }
}

void onSetBux(Bot *bot, const utils::VariantList &variant) {
  int32_t gems = variant.get(1)->as_int32();
  bot->state.gems = gems;
}

void onConsoleMessage(Bot *bot, const utils::VariantList &variant) {
  std::string message = variant.get(1)->as_string();
  spdlog::info("Received console message: {}", message);
}

void onSetPos(Bot *bot, const utils::VariantList &variant) {
  utils::Variant::Vec2 pos = variant.get(1)->as_vec2();
  bot->position.x = pos.first;
  bot->position.y = pos.second;
}

void onFtueButtonDataSet(Bot *bot, const utils::VariantList &variant) {
  int32_t unknown = variant.get(1)->as_int32();
  int32_t current_progress = variant.get(2)->as_int32();
  int32_t total_progress = variant.get(3)->as_int32();
  std::string info = variant.get(4)->as_string();
  spdlog::info("FTUE Button Data Set: {} {} {} {}", unknown, current_progress,
               total_progress, info);
}

void onSpawn(Bot *bot, const utils::VariantList &variant) {
  std::string message = variant.get(1)->as_string();
  std::unordered_map<std::string, std::string> data =
      utils::TextParse::parse_and_store_as_map(message);

  bot->state.is_ingame = true;
  bot->state.net_id = std::stoi(data["netID"]);
}

void setHasGrowID(Bot *bot, const utils::VariantList &variant) {
  bot->info.display_name = variant.get(2)->as_string();
}

void Variant::handle(Bot *bot, uint8_t *data) {
  const utils::VariantList variant = utils::VariantList::deserialize(data);
  const std::string function_call = variant.get(0)->as_string();
  spdlog::info("Received function call: {}", function_call);

  std::unordered_map<std::string,
                     std::function<void(Bot *, utils::VariantList)>>
      handlers = {{std::string("OnSendToServer"), onSendToServer},
                  {std::string("OnSuperMainStartAcceptLogonHrdxs47254722215a"),
                   onSuperMainStartAcceptLogonHrdxs47254722215a},
                  {std::string("OnCountryState"), onCountryState},
                  {std::string("OnDialogRequest"), onDialogRequest},
                  {std::string("OnSetBux"), onSetBux},
                  {std::string("OnConsoleMessage"), onConsoleMessage},
                  {std::string("OnSetPos"), onSetPos},
                  {std::string("OnFtueButtonDataSet"), onFtueButtonDataSet},
                  {std::string("OnSpawn"), onSpawn},
                  {std::string("SetHasGrowID"), setHasGrowID}};

  if (handlers.find(function_call) != handlers.end()) {
    handlers[function_call](bot, variant);
  }
}