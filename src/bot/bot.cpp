#include "bot.hpp"
#include <spdlog/spdlog.h>
#include <cpr/cpr.h>
#include <iostream>
#include <regex>

#include "utils/textparse.hpp"
#include "utils/proton.hpp">

Bot::Bot(const std::string& username, const std::string& password, const types::ELoginMethod method)
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
	this->get_oauth_links();
	this->login_info.klv = utils::generate_klv(this->login_info.protocol, this->login_info.game_version, this->login_info.rid);
	this->login_info.hash = std::to_string(utils::hash_string(fmt::format("{}RT", this->login_info.mac)));
	this->login_info.hash2 = std::to_string(utils::hash_string(fmt::format("{}RT", this->login_info.rid)));
}

void Bot::to_http()
{
	cpr::Response r = cpr::Post(cpr::Url{ "https://www.growtopia1.com/growtopia/server_data.php" }, cpr::UserAgent{ "UbiServices_SDK_2022.Release.9_PC64_ansi_static" }, cpr::Body{ "" });
	this->server_data = TextParse::parse_and_store_as_map(r.text);
}

void Bot::get_oauth_links()
{
	cpr::Response r = cpr::Post(cpr::Url{ "https://login.growtopiagame.com/player/login/dashboard" }, cpr::UserAgent{ this->user_agent }, cpr::Body{ fmt::format("tankIDName|\ntankIDPass|\nrequestedName|BoardSickle\nf|1\nprotocol|209\ngame_version|4.62\nfz|41745432\nlmode|0\ncbits|1040\nplayer_age|20\nGDPR|3\ncategory|_-5100\ntotalPlaytime|0\nklv|b351d8dacd7a776848b31c74d3d550ec61dbb9b96c3ac67aea85034a84401a87\nhash2|841545814\nmeta|{}\nfhash|-716928004\nrid|01F9EBD204B52C940285667E15C00D62\nplatformID|0,1,1\ndeviceVersion|0\ncountry|us\nhash|-1829975549\nmac|b4:8c:9d:90:79:cf\nwk|66A6ABCD9753A066E39975DED77852A8\nzf|617169524\n", this->server_data["meta"]) });
	const std::regex pattern("https:\\/\\/login\\.growtopiagame\\.com\\/(apple|google|player\\/growid)\\/(login|redirect)\\?token=[^\"]+");

	auto matches_begin = std::sregex_iterator(r.text.begin(), r.text.end(), pattern);
	auto matches_end = std::sregex_iterator();

	for (std::sregex_iterator i = matches_begin; i != matches_end; ++i) {
		const std::smatch& match = *i;
		this->oauth_links.push_back(match.str());
	}
}
