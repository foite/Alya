cmake_minimum_required(VERSION 3.0)
project(Alya)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)
set(JSON_BuildTests OFF CACHE INTERNAL "")

find_package(OpenSSL REQUIRED)

add_subdirectory(thirdparty/cpr)
add_subdirectory(thirdparty/enet)
add_subdirectory(thirdparty/fmt)
add_subdirectory(thirdparty/magic_enum)
add_subdirectory(thirdparty/spdlog)
add_subdirectory(thirdparty/json)

include_directories(src)
include_directories(thirdparty/enet/include)

add_executable(${PROJECT_NAME} src/main.cpp "src/manager/manager.cpp" "src/bot/bot.cpp" "src/utils/textparse.cpp" "src/bot/packet/packet_handler.cpp" "src/bot/login/login.cpp")
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json OpenSSL::SSL cpr::cpr enet fmt::fmt magic_enum::magic_enum spdlog::spdlog)