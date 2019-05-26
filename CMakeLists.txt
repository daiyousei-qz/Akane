cmake_minimum_required(VERSION 3.10)

project(Akane CXX)

set(CMAKE_CXX_STANDARD 17)

include(ExternalProject)
include(cmake/FindEmbree.cmake)
include(cmake/FindJson.cmake)
include(cmake/FindFmtlib.cmake)
include(cmake/FindSpdlog.cmake)
include(cmake/FindTinyObjLoader.cmake)
include(cmake/FindQuickImGui.cmake)

add_subdirectory(./akane-core)
add_subdirectory(./akane-gui)
add_subdirectory(./akane-shell)