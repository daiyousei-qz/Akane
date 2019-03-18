cmake_minimum_required(VERSION 3.8)

file(GLOB_RECURSE AKANE_CORE_SOURCES ./src/*.cpp)
add_library(akene-core SHARED ${AKANE_CORE_SOURCES})

target_include_directories(akene-core 
	PUBLIC ./include 
	PRIVATE ./src)