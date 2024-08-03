cmake_minimum_required(VERSION 3.0)
project(Alya)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(thirdparty/cpr)
add_subdirectory(thirdparty/enet)
add_subdirectory(thirdparty/fmt)
add_subdirectory(thirdparty/magic_enum)
add_subdirectory(thirdparty/spdlog)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} cpr::cpr enet fmt::fmt magic_enum::magic_enum spdlog::spdlog)