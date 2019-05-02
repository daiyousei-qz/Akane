
if(AKANE_EXTERNAL_EMBREE_INCLUDED)
	return()
endif()
set(AKANE_EXTERNAL_EMBREE_INCLUDED TRUE)

add_library(embree INTERFACE)
add_library(external::embree ALIAS embree)

set(EMBREE_LIBRARY_DIR "${CMAKE_SOURCE_DIR}/embree/windows")

target_include_directories(embree INTERFACE ${EMBREE_LIBRARY_DIR}/include)

target_link_libraries(embree INTERFACE
	${EMBREE_LIBRARY_DIR}/lib/embree3.lib
	${EMBREE_LIBRARY_DIR}/lib/embree_avx.lib
	${EMBREE_LIBRARY_DIR}/lib/embree_avx2.lib
	${EMBREE_LIBRARY_DIR}/lib/embree_sse42.lib
	${EMBREE_LIBRARY_DIR}/lib/lexers.lib
	${EMBREE_LIBRARY_DIR}/lib/math.lib
	${EMBREE_LIBRARY_DIR}/lib/simd.lib
	${EMBREE_LIBRARY_DIR}/lib/sys.lib
	${EMBREE_LIBRARY_DIR}/lib/tasking.lib)
