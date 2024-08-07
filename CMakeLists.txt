cmake_minimum_required(VERSION 3.2)
project(Alya)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)
set(JSON_BuildTests OFF CACHE INTERNAL "")

find_package(OpenSSL REQUIRED)
file(COPY ${CMAKE_SOURCE_DIR}/items.dat DESTINATION ${CMAKE_BINARY_DIR})

add_subdirectory(thirdparty/cpr)
add_subdirectory(thirdparty/enet)
add_subdirectory(thirdparty/fmt)
add_subdirectory(thirdparty/magic_enum)
add_subdirectory(thirdparty/spdlog)
add_subdirectory(thirdparty/json)

include_directories(src)
include_directories(thirdparty/enet/include)

file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json enet OpenSSL::SSL cpr::cpr fmt::fmt magic_enum::magic_enum spdlog::spdlog)