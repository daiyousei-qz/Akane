cmake_minimum_required(VERSION 3.10)

file(GLOB_RECURSE AKANE_GUI_SOURCES ./src/*.cpp)
add_executable(akane-gui WIN32 ${AKANE_GUI_SOURCES})

set_property(TARGET akane-gui PROPERTY CXX_STANDARD 17)

target_include_directories(akane-gui
    PRIVATE ./src)

target_link_libraries(akane-gui
	PRIVATE akane-core external::quick-imgui d3d11)
