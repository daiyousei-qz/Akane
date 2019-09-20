if(AKANE_EXTERNAL_STB_INCLUDED)
	return()
endif()
set(AKANE_EXTERNAL_STB_INCLUDED TRUE)

# TODO: verify there's stb
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${CMAKE_SOURCE_DIR}/external/stb)

add_library(external::stb ALIAS stb)
