if(AKANE_EXTERNAL_FMT_INCLUDED)
	return()
endif()
set(AKANE_EXTERNAL_FMT_INCLUDED TRUE)

add_subdirectory(${CMAKE_SOURCE_DIR}/external/fmt)

add_library(external::fmt ALIAS fmt)
