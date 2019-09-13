cmake_minimum_required(VERSION 3.10)

file(GLOB_RECURSE AKANE_CORE_SOURCES ./src/*.cpp)
add_executable(akane-core ${AKANE_CORE_SOURCES})

target_include_directories(akane-core 
	PUBLIC ./include
	PRIVATE ./src)

target_link_libraries(akane-core
	PUBLIC fmt::fmt stb)