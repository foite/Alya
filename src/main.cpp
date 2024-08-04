#include <iostream>
#include "manager/manager.hpp"

using namespace types;

int main()
{
	Manager manager{};
	manager.add_bot("", "", ELoginMethod::LEGACY);
	return 0;
}