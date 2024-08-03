#include <iostream>
#include "manager/manager.hpp"

int main()
{
	Manager manager{};
	manager.add_bot("", "", ELoginMethod::LEGACY);
	std::cout << "Hello, World!" << "\n";
	return 0;
}