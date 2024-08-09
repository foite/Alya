#pragma once
#include <string>

class Login {
public:
  static std::string get_legacy_token(std::string &url, std::string &username,
                                      std::string &password);
  static std::string get_google_token(std::string &url, std::string &username,
                                      std::string &password);

private:
  static std::string extract_token_from_html(std::string &body);
};