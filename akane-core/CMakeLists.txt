cmake_minimum_required(VERSION 3.12)

file(GLOB_RECURSE AKANE_CORE_SOURCES ./src/*.cpp)
#add_library(akene-core SHARED)
add_executable(akene-core)
target_sources(akene-core PRIVATE ${AKANE_CORE_SOURCES})

target_include_directories(akene-core 
	PUBLIC ./include
	PRIVATE ./include-internal)