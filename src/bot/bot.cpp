#include "bot.hpp"
#include <spdlog/spdlog.h>
#include <cpr/cpr.h>
#include <iostream>

Bot::Bot(std::string username, std::string password, ELoginMethod method)
{
	this->username = std::move(username);
	this->password = std::move(password);
	this->method = method;

	this->login();
}

void Bot::login()
{
	if (enet_initialize() != 0)
	{
		spdlog::error("An error occurred while initializing ENet.");
	}
	this->to_http();
}

void Bot::to_http()
{
	cpr::Response r = cpr::Post(cpr::Url{ "https://www.growtopia1.com/growtopia/server_data.php" }, cpr::UserAgent{ "UbiServices_SDK_2022.Release.9_PC64_ansi_static" }, cpr::Body{ "" });
	std::cout << r.text << std::endl;
}

void Bot::get_oauth_links()
{
}
