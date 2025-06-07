cmake_minimum_required(VERSION 3.15)
project(emulator)

# Choose C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Debug flag (can be overridden with -DDEBUG=ON)
option(DEBUG "Enable debug mode" OFF)

if(DEBUG)
    message(STATUS "Building with debugging info")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g")
else()
    message(STATUS "Building with optimizations")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif()

# Source files
file(GLOB_RECURSE SRC_FILES "source/*.cpp")
file(GLOB IMGUI_SRC "include/imgui/source/*.cpp")
set(MAIN_FILE "main.cpp")

# Add executable target
add_executable(emulator ${MAIN_FILE} ${SRC_FILES} ${IMGUI_SRC})

# Include directories
target_include_directories(emulator PRIVATE include/imgui/header)

# Link SDL2 and SDL2_mixer (assumes installed via system package manager)
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)

target_link_libraries(emulator SDL2::SDL2 SDL2_mixer::SDL2_mixer)
