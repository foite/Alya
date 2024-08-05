#include "manager/manager.hpp"
#include <iostream>

using namespace types;

int main() {
  Manager manager{};
  manager.add_bot("", "", "", ELoginMethod::LEGACY);
  std::cout << "Hello world" << std::endl;
  return 0;
}