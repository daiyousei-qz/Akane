if(AKANE_EXTERNAL_EDSLIB_INCLUDED)
	return()
endif()
set(AKANE_EXTERNAL_EDSLIB_INCLUDED TRUE)

add_subdirectory(${CMAKE_SOURCE_DIR}/external/edslib)

add_library(external::edslib ALIAS edslib)
