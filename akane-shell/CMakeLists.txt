cmake_minimum_required(VERSION 3.10)

add_executable(akane-shell ./src/main.cpp)

target_link_libraries(akane-shell
	PRIVATE akane-core)