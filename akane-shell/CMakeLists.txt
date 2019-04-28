cmake_minimum_required(VERSION 3.12)

add_executable(akane-shell)
target_sources(akane-shell 
	PRIVATE ./src/main.cpp)

target_link_libraries(akane-shell
	PRIVATE akane-core)