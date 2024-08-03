#pragma once
#include <memory>
#include <string>
#include <enet/enet.h>
#include "types/eloginmethod.hpp"

class Bot
{
public:
	Bot(std::string username, std::string password, ELoginMethod method);
	void disconnect();
	void punch();
	void place();
	void warp();
	void talk();
	void send_packet();
private:
	void login();
	void to_http();
	void get_oauth_links();
	void parse_server_data();
public:
	std::string user_agent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36 Edg/120.0.0.0";
	std::string username;
	std::string password;
	ELoginMethod method;
	std::unique_ptr<ENetPeer> peer;
	std::unique_ptr<ENetHost> host;
};
