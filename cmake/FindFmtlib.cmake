
if(AKANE_EXTERNAL_FMTLIB_INCLUDED)
	return()
endif()
set(AKANE_EXTERNAL_FMTLIB_INCLUDED TRUE)

add_subdirectory(${CMAKE_SOURCE_DIR}/external/fmtlib)

add_library(external::fmtlib ALIAS fmt)
