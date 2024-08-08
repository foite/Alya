#include "login.hpp"
#include "cpr/cpr.h"
#include "spdlog/spdlog.h"
#include <iostream>
#include <nlohmann/json.hpp>
#include <regex>
#include <string>

static std::string USER_AGENT =
    "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 "
    "(KHTML, like Gecko) Chrome/126.0.0.0 Safari/537.36";

std::string Login::get_legacy_token(std::string &url, std::string &username,
                                    std::string &password) {
  while (true) {
    cpr::Session session;
    session.SetUrl(cpr::Url{url});
    session.SetUserAgent(USER_AGENT);
    cpr::Response r = session.Get();

    if (r.status_code != 200) {
      spdlog::error("Failed to get token, retrying...");
      continue;
    }

    std::string token = extract_token_from_html(r.text);

    session.SetUrl(cpr::Url{
        "https://login.growtopiagame.com/player/growid/login/validate"});
    session.SetPayload(
        {{"_token", token}, {"growId", username}, {"password", password}});
    r = session.Post();

    if (r.status_code != 200) {
      spdlog::error("Failed to validate token, retrying...");
      continue;
    }

    nlohmann::json j = nlohmann::json::parse(r.text);
    if (j["status"] == "success") {
      return j["token"];
    }
  }
}

std::string Login::extract_token_from_html(std::string &body) {
  const std::regex pattern(
      "name=\"_token\"\\s+type=\"hidden\"\\s+value=\"([^\"]*)\"");
  auto matches_begin = std::sregex_iterator(body.begin(), body.end(), pattern);
  auto matches_end = std::sregex_iterator();

  for (std::sregex_iterator i = matches_begin; i != matches_end; ++i) {
    const std::smatch &match = *i;
    return match.str(1);
  }
  return "";
}