
set(JSON_BuildTests OFF)
set(JSON_Install OFF)
set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/json)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/json)

add_library(external::json ALIAS nlohmann_json)
