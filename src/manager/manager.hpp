#pragma once
#include <string>
#include <vector>
#include "types/eloginmethod.hpp"
#include "bot/bot.hpp"

class Manager
{
public:
	void add_bot(std::string username, std::string password, ELoginMethod method);
	void get_bot(std::string username);
	void remove_bot(std::string username);

	std::vector<std::shared_ptr<Bot>> bots;
};