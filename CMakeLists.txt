cmake_minimum_required(VERSION 3.0)
project(Alya)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)

find_package(OpenSSL REQUIRED)

add_subdirectory(thirdparty/cpr)
add_subdirectory(thirdparty/enet)
add_subdirectory(thirdparty/fmt)
add_subdirectory(thirdparty/magic_enum)
add_subdirectory(thirdparty/spdlog)

include_directories(src)
include_directories(thirdparty/enet/include)

add_executable(${PROJECT_NAME} src/main.cpp "src/manager/manager.hpp" "src/manager/manager.cpp" "src/bot/bot.cpp" "src/utils/textparse.cpp")
target_link_libraries(${PROJECT_NAME} OpenSSL::SSL cpr::cpr enet fmt::fmt magic_enum::magic_enum spdlog::spdlog)