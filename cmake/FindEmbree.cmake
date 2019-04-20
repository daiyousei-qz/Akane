

add_library(embree INTERFACE)
add_library(external::embree ALIAS embree)

target_include_directories(embree INTERFACE ${CMAKE_SOURCE_DIR}/embree/include)
target_link_libraries(embree INTERFACE
	${CMAKE_SOURCE_DIR}/embree/lib/embree_avx.lib
	${CMAKE_SOURCE_DIR}/embree/lib/embree_avx2.lib
	${CMAKE_SOURCE_DIR}/embree/lib/embree_sse42.lib
	${CMAKE_SOURCE_DIR}/embree/lib/embree3.lib
	${CMAKE_SOURCE_DIR}/embree/lib/lexers.lib
	${CMAKE_SOURCE_DIR}/embree/lib/math.lib
	${CMAKE_SOURCE_DIR}/embree/lib/simd.lib
	${CMAKE_SOURCE_DIR}/embree/lib/sys.lib
	${CMAKE_SOURCE_DIR}/embree/lib/tasking.lib)