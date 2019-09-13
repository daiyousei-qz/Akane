cmake_minimum_required(VERSION 3.10)

project(Akane CXX)

set(CMAKE_CXX_STANDARD 17)

# ==============================================================================
# Load Dependency

# fmt::fmt
find_package(fmt REQUIRED)

# spdlog::spdlog
find_package(spdlog REQUIRED)

# stb
find_path(STB_INCLUDE_DIR NAMES stb.h)
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${STB_INCLUDE_DIR})

# Catch2::Catch2
find_package(Catch2 REQUIRED)

# ==============================================================================

add_subdirectory(./akane-core)