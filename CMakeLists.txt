cmake_minimum_required(VERSION 3.2)
project(Alya)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)
set(JSON_BuildTests OFF CACHE INTERNAL "")

find_package(OpenSSL REQUIRED)
find_package(OpenGL REQUIRED)
file(COPY ${CMAKE_SOURCE_DIR}/items.dat DESTINATION ${CMAKE_BINARY_DIR})

add_subdirectory(thirdparty/cpr)
add_subdirectory(thirdparty/fmt)
add_subdirectory(thirdparty/magic_enum)
add_subdirectory(thirdparty/spdlog)
add_subdirectory(thirdparty/json)
add_subdirectory(thirdparty/glfw)

include_directories(src)
include_directories(thirdparty/imgui)
include_directories(thirdparty/enet/include)

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB IMGUI_SOURCES 
    thirdparty/imgui/*.cpp 
    thirdparty/imgui/*.h 
    thirdparty/imgui/backends/imgui_impl_glfw.* 
    thirdparty/imgui/backends/imgui_impl_opengl3.* 
    thirdparty/imgui/misc/cpp/imgui_stdlib.*
)
file(GLOB ENET_SOURCES 
    thirdparty/enet/*.c
)

add_executable(${PROJECT_NAME} ${SOURCES} ${IMGUI_SOURCES} ${ENET_SOURCES})
target_link_libraries(${PROJECT_NAME} nlohmann_json::nlohmann_json winmm OpenGL::GL glfw OpenSSL::SSL cpr::cpr fmt::fmt magic_enum::magic_enum spdlog::spdlog)