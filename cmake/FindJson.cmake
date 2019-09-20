if(AKANE_EXTERNAL_JSON_INCLUDED)
	return()
endif()
set(AKANE_EXTERNAL_JSON_INCLUDED TRUE)

set(JSON_BuildTests OFF)
set(JSON_Install OFF)
set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/nlohmann-json)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/nlohmann-json)

add_library(external::json ALIAS nlohmann_json)
