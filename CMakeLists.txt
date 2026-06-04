cmake_minimum_required(VERSION 3.10)
project(brredit VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)

file(GLOB CORE_SRC "src/Core/*.cc")
file(GLOB EDITOR_SRC "src/Editor/*.cc")
file(GLOB PLAYER_SRC "src/Player/*.cc")

add_library(brr_core STATIC ${CORE_SRC})

add_executable(brredit ${EDITOR_SRC})
add_executable(brr_player ${PLAYER_SRC})

add_library(imgui lib/imgui/imgui.cpp lib/imgui/imgui_demo.cpp lib/imgui/imgui_draw.cpp lib/imgui/imgui_tables.cpp lib/imgui/imgui_widgets.cpp)

include_directories(src lib/raylib/src lib/imgui lib/rlImGui lib)

add_library(rlImGui lib/rlImGui/rlImGui.cpp)

add_subdirectory(lib/raylib)

target_link_libraries(brr_core PRIVATE raylib)

target_link_libraries(brredit PRIVATE m pthread raylib imgui rlImGui brr_core)
target_link_libraries(brr_player PRIVATE m pthread raylib brr_core)
