cmake_minimum_required(VERSION 3.16)

SET(OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
SET(LIB_OUTPUT_DIR "${CMAKE_BINARY_DIR}/lib")
SET(GAME_PROJECT game)
SET(ASSET_LOADER_PROJECT asset_loader)
SET(DEBUGGING_PROJECT debugging)

project(${GAME_PROJECT} LANGUAGES C CXX)

include(${PROJECT_SOURCE_DIR}/cmake/CompilerWarnings.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/clangformat.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/cppcheck.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/vk.cmake)
include(FetchContent)
include(cmake/dependencies.cmake)

fetch_third_party()

add_subdirectory("src")