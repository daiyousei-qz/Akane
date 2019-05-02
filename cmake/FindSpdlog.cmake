
if(AKANE_EXTERNAL_SPDLOG_INCLUDED)
	return()
endif()
set(AKANE_EXTERNAL_SPDLOG_INCLUDED TRUE)

if(NOT AKANE_EXTERNAL_FMTLIB_INCLUDED)
	include(${CMAKE_SOURCE_DIR}/cmake/FindFmtlib.cmake)
endif()

set(SPDLOG_FMT_EXTERNAL ON)
set(SPDLOG_FMT_HEADER_ONLY OFF)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/spdlog)

add_library(external::spdlog ALIAS spdlog)
