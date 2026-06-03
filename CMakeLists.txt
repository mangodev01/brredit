cmake_minimum_required(VERSION 3.10)
project(brredit VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)

file(GLOB_RECURSE SRC "src/*.cc")

add_executable(brredit ${SRC})

add_library(imgui lib/imgui/imgui.cpp lib/imgui/imgui_demo.cpp lib/imgui/imgui_draw.cpp lib/imgui/imgui_tables.cpp lib/imgui/imgui_widgets.cpp)

# Example: adding include directories
include_directories(lib/raylib/src lib/imgui lib/rlImGui lib)

add_library(rlImGui lib/rlImGui/rlImGui.cpp)

# Example: add git submodule that has CMakeLists.txt
add_subdirectory(lib/raylib)

# Example: linking libraries
target_link_libraries(brredit PRIVATE m pthread raylib imgui rlImGui)
