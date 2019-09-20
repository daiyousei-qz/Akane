
if(AKANE_EXTERNAL_TINYOBJLOADER_INCLUDED)
	return()
endif()
set(AKANE_EXTERNAL_TINYOBJLOADER_INCLUDED TRUE)

add_subdirectory(${CMAKE_SOURCE_DIR}/external/tinyobjloader)

add_library(external::tinyobjloader ALIAS tinyobjloader)
