cmake_minimum_required(VERSION 3.10)

project(Akane CXX)

set(CMAKE_CXX_STANDARD 17)

if(MSVC)
    add_compile_options(/Zc:__cplusplus)
endif()

# ==============================================================================
# Load Dependency

include(cmake/FindEmbree.cmake)
include(cmake/FindEdsLib.cmake)
include(cmake/FindFmt.cmake)
include(cmake/FindJson.cmake)
include(cmake/FindQuickImGui.cmake)
include(cmake/FindStb.cmake)
include(cmake/FindTinyObjLoader.cmake)

# ==============================================================================

add_subdirectory(./akane-core)
add_subdirectory(./akane-gui)
add_subdirectory(./akane-shell)