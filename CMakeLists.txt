cmake_minimum_required(VERSION 3.16)

SET(OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
SET(LIB_OUTPUT_DIR "${CMAKE_BINARY_DIR}/lib")
SET(GAME_TARGET game)
SET(ASSET_LOADER_PROJECT asset_loader)
SET(DEBUGGING_PROJECT debugging)
SET(ENGINE_TARGET engine)

project("Odin" LANGUAGES C CXX)

include(${PROJECT_SOURCE_DIR}/cmake/CompilerWarnings.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/clangformat.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/cppcheck.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/vk.cmake)
include(FetchContent)
include(cmake/dependencies.cmake)

fetch_third_party()

# Configure and compile external dependencies first
set(SDL_SHARED          OFF CACHE BOOL "Build SDL as a shared library" FORCE)
set(SDL_STATIC          ON CACHE BOOL "Build SDL as a static library" FORCE)
set(SDL_RENDER          OFF CACHE BOOL "" FORCE)
set(SDL_RENDER_D3D      OFF CACHE BOOL "" FORCE)
set(SDL_RENDER_D3D11    OFF CACHE BOOL "" FORCE)
set(SDL_RENDER_D3D12    OFF CACHE BOOL "" FORCE)
set(SDL_RENDER_GPU      OFF CACHE BOOL "" FORCE)
set(SDL_RENDER_METAL    OFF CACHE BOOL "" FORCE)
set(SDL_RENDER_VULKAN   OFF CACHE BOOL "" FORCE)
set(SDL_OPENGL          OFF CACHE BOOL "" FORCE)
set(SDL_OPENGLES        OFF CACHE BOOL "" FORCE)
set(SDL_SENSOR        OFF CACHE BOOL "" FORCE)
add_subdirectory("external/sdl")

add_subdirectory("src")