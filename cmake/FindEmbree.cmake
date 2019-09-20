
if(AKANE_EXTERNAL_EMBREE_INCLUDED)
	return()
endif()
set(AKANE_EXTERNAL_EMBREE_INCLUDED TRUE)

find_package(embree 3.0 REQUIRED)

add_library(embree INTERFACE)
target_include_directories(embree INTERFACE ${EMBREE_INCLUDE_DIRS})
target_link_libraries(embree INTERFACE ${EMBREE_LIBRARY})

add_library(external::embree ALIAS embree)
