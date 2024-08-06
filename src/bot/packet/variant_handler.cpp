#include "variant_handler.hpp"
#include "types/e_packet_type.hpp"
#include "utils/textparse.hpp"
#include "utils/variant.hpp"
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

void Variant::handle(Bot *bot, uint8_t *data) {
  const utils::VariantList variant = utils::VariantList::deserialize(data);
  const std::string function_call = variant.get(0)->as_string();
  spdlog::info("Received function call: {}", function_call);

  if (function_call == "OnSendToServer") {
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
  if (function_call == "OnSuperMainStartAcceptLogonHrdxs47254722215a") {
    bot->send_packet(types::EPacketType::NetMessageGenericText,
                     "action|enter_game\n");
    bot->state.is_redirect = false;
  }
  if (function_call == "OnCountryState") {
    bot->send_packet(types::EPacketType::NetMessageGenericText,
                     "action|getDRAnimations\n");
    bot->send_packet(types::EPacketType::NetMessageGenericText,
                     "action|getDRAnimations\n");
  }
  if (function_call == "OnDialogRequest") {
    std::string message = variant.get(1)->as_string();
    if (message.find("Gazette") != std::string::npos) {
      bot->send_packet(
          types::EPacketType::NetMessageGenericText,
          "action|dialog_return\ndialog_name|gazette\nbuttonClicked|banner\n");
    }
  }
  if (function_call == "OnSetBux") {
    int32_t gems = variant.get(1)->as_int32();
    bot->state.gems = gems;
  }
  if (function_call == "OnConsoleMessage") {
    std::string message = variant.get(1)->as_string();
    spdlog::info("Received console message: {}", message);
  }
  if (function_call == "OnSetPos") {
    utils::Variant::Vec2 pos = variant.get(1)->as_vec2();
    bot->position.x = pos.first;
    bot->position.y = pos.second;
  }
  if (function_call == "OnFtueButtonDataSet") {
    int32_t unknown = variant.get(1)->as_int32();
    int32_t current_progress = variant.get(2)->as_int32();
    int32_t total_progress = variant.get(3)->as_int32();
    std::string info = variant.get(4)->as_string();
    spdlog::info("FTUE Button Data Set: {} {} {} {}", unknown, current_progress,
                 total_progress, info);
  }
  if (function_call == "OnSpawn") {
    std::string message = variant.get(1)->as_string();
    std::unordered_map<std::string, std::string> data =
        utils::TextParse::parse_and_store_as_map(message);

    bot->state.is_ingame = true;
    bot->state.net_id = std::stoi(data["netID"]);
  }
  if (function_call == "SetHasGrowID") {
    bot->info.display_name = variant.get(2)->as_string();
  }
}