cmake_minimum_required(VERSION 3.10)

file(GLOB_RECURSE AKANE_GUI_SOURCES ./src/*.cpp ./include/external/imgui/*.cpp)

add_executable(akane-gui WIN32 ${AKANE_GUI_SOURCES})

target_include_directories(akane-gui
	PRIVATE ./include)

target_link_libraries(akane-gui
	PRIVATE akane-core d3d11)

#target_link_libraries(tut1 d3d10 d3dx10 dxguid)