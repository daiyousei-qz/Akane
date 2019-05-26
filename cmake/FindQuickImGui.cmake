if(AKANE_EXTERNAL_QUICKIMGUI_INCLUDED)
	return()
endif()
set(AKANE_EXTERNAL_QUICKIMGUI_INCLUDED TRUE)

add_subdirectory(${CMAKE_SOURCE_DIR}/external/QuickImGui)

add_library(external::quick-imgui ALIAS quick-imgui)