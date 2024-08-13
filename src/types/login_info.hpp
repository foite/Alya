#pragma once
#include "utils/random.hpp"
#include <sstream>
#include <string>

namespace types {
struct LoginInfo {
  std::string uuid;
  std::string protocol = "209";
  std::string fhash = "-716928004";
  std::string mac = utils::random_mac_address();
  std::string requested_name = "BraveDuck";
  std::string hash2;
  std::string fz = "47142936";
  std::string f = "1";
  std::string player_age = "20";
  std::string game_version = "4.63";
  std::string lmode = "1";
  std::string cbits = "1040";
  std::string rid = utils::random_hex(32, true);
  std::string gdpr = "3";
  std::string hash = "0";
  std::string category = "_-5100";
  std::string token;
  std::string total_playtime = "0";
  std::string door_id;
  std::string klv;
  std::string meta;
  std::string platform_id = "0,1,1";
  std::string device_version = "0";
  std::string zf = "-821693372";
  std::string country = "jp";
  std::string user;
  std::string wk = utils::random_hex(32, true);

  std::string to_string() const {
    std::ostringstream oss;
    oss << "UUIDToken|" << uuid << "\n"
        << "protocol|" << protocol << "\n"
        << "fhash|" << fhash << "\n"
        << "mac|" << mac << "\n"
        << "requestedName|" << requested_name << "\n"
        << "hash2|" << hash2 << "\n"
        << "fz|" << fz << "\n"
        << "f|" << f << "\n"
        << "player_age|" << player_age << "\n"
        << "game_version|" << game_version << "\n"
        << "lmode|" << lmode << "\n"
        << "cbits|" << cbits << "\n"
        << "rid|" << rid << "\n"
        << "GDPR|" << gdpr << "\n"
        << "hash|" << hash << "\n"
        << "category|" << category << "\n"
        << "token|" << token << "\n"
        << "total_playtime|" << total_playtime << "\n"
        << "door_id|" << door_id << "\n"
        << "klv|" << klv << "\n"
        << "meta|" << meta << "\n"
        << "platformID|" << platform_id << "\n"
        << "deviceVersion|" << device_version << "\n"
        << "zf|" << zf << "\n"
        << "country|" << country << "\n"
        << "user|" << user << "\n"
        << "wk|" << wk << "\n";
    return oss.str();
  }
};
} // namespace types