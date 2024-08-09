#include "variant_handler.hpp"
#include "types/e_packet_type.hpp"
#include "utils/textparse.hpp"
#include "utils/variant.hpp"
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

void onSendToServer(Bot *bot, utils::variantlist_t variant) {
  int32_t port = variant[1].get_int32();
  int32_t token = variant[2].get_int32();
  int32_t user_id = variant[3].get_int32();
  std::string server_data = variant[4].get_string();
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
void onSuperMainStartAcceptLogonHrdxs47254722215a(
    Bot *bot, utils::variantlist_t variant) {
  bot->send_packet(types::EPacketType::NetMessageGenericText,
                   "action|enter_game\n");
  bot->state.is_redirect = false;
}
void onCountryState(Bot *bot, utils::variantlist_t variant) {
  bot->send_packet(types::EPacketType::NetMessageGenericText,
                   "action|getDRAnimations\n");
  bot->send_packet(types::EPacketType::NetMessageGenericText,
                   "action|getDRAnimations\n");
}
void onDialogRequest(Bot *bot, utils::variantlist_t variant) {
  std::string message = variant[1].get_string();
  if (message.find("Gazette") != std::string::npos) {
    bot->send_packet(
        types::EPacketType::NetMessageGenericText,
        "action|dialog_return\ndialog_name|gazette\nbuttonClicked|banner\n");
  }
}
void onSetBux(Bot *bot, utils::variantlist_t variant) {
  int32_t gems = variant[1].get_int32();
  bot->state.gems = gems;
}
void onConsoleMessage(Bot *bot, utils::variantlist_t variant) {
  std::string message = variant[1].get_string();
  spdlog::info("Received console message: {}", message);
}
void onSetPos(Bot *bot, utils::variantlist_t variant) {
  utils::vector2_t pos = variant[1].get_vector2();
  bot->position.x = pos.m_x;
  bot->position.y = pos.m_y;
  // this used to place first world lock
  // if (bot->state.is_ingame) {
  //   bot->place(0, -1, 9640);
  // }
}
void onFtueButtonDataSet(Bot *bot, utils::variantlist_t variant) {
  int32_t unknown = variant[1].get_int32();
  int32_t current_progress = variant[2].get_int32();
  int32_t total_progress = variant[3].get_int32();
  std::string info = variant[4].get_string();
  spdlog::info("FTUE Button Data Set: {} {} {} {}", unknown, current_progress,
               total_progress, info);
}
void onSpawn(Bot *bot, utils::variantlist_t variant) {
  std::string message = variant[1].get_string();
  std::unordered_map<std::string, std::string> data =
      utils::TextParse::parse_and_store_as_map(message);

  bot->state.is_ingame = true;
  bot->state.net_id = std::stoi(data["netID"]);
}
void setHasGrowID(Bot *bot, utils::variantlist_t variant) {
  bot->info.display_name = variant[2].get_string();
}

void onTalkBubble(Bot *bot, utils::variantlist_t variant) {
  std::string message = variant[2].get_string();
  spdlog::info("Received talk bubble: {}", message);
}

void Variant::handle(Bot *bot, uint8_t *data) {
  utils::variantlist_t variant{};
  variant.serialize_from_mem(data);
  const std::string function_call = variant[0].get_string();
  spdlog::info("Received function call: {}", function_call);

  std::unordered_map<std::string,
                     std::function<void(Bot *, utils::variantlist_t)>>
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
                  {std::string("SetHasGrowID"), setHasGrowID},
                  {std::string("OnTalkBubble"), onTalkBubble}};

  if (handlers.find(function_call) != handlers.end()) {
    handlers[function_call](bot, variant);
  }
}