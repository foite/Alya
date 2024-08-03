#include "manager.hpp"

void Manager::add_bot(std::string username, std::string password, ELoginMethod method)
{
	bots.push_back(std::make_shared<Bot>(std::move(username), std::move(password), method));
}
