cmake_minimum_required(VERSION 3.10)

file(GLOB_RECURSE AKANE_CORE_INTERNAL_HEADERS ./src/*.h)
file(GLOB_RECURSE AKANE_CORE_SOURCES ./src/*.cpp)
add_library(akane-core ${AKANE_CORE_INTERNAL_HEADERS} ${AKANE_CORE_SOURCES})

target_include_directories(akane-core 
    PUBLIC ./include
    PUBLIC ./src)
    #PRIVATE ./src)

target_link_libraries(akane-core
	PUBLIC external::embree 
           external::edslib 
           external::tinyobjloader 
           external::fmt 
           external::stb 
           external::json)