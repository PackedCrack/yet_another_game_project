cmake_minimum_required(VERSION 3.16)

SET(OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
SET(LIB_OUTPUT_DIR "${CMAKE_BINARY_DIR}/lib")
SET(GAME_TARGET game)
SET(ASSET_LOADER_TARGET asset_loader)
SET(DEBUG_TARGET debugging)
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
set(SDL_SENSOR          OFF CACHE BOOL "" FORCE)
add_subdirectory("external/sdl")

set_target_properties(SDL3-static SDL3_test SDL_uclibc PROPERTIES FOLDER "_external_SDL3")


if(NOT EXISTS "${PROJECT_SOURCE_DIR}/external/shaderc/third_party/spirv-tools/CMakeLists.txt")
    find_package(Python3 COMPONENTS Interpreter REQUIRED)

    execute_process(COMMAND "${Python3_EXECUTABLE}" "${PROJECT_SOURCE_DIR}/external/shaderc/utils/git-sync-deps"
                    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/external/shaderc/utils")
endif()

set(SHADERC_ENABLE_SHARED_CRT       ON CACHE BOOL "" FORCE)
set(SHADERC_SKIP_COPYRIGHT_CHECK    ON CACHE BOOL "" FORCE)
set(SHADERC_SKIP_TESTS              ON CACHE BOOL "" FORCE)
set(SHADERC_SKIP_EXAMPLES           ON CACHE BOOL "" FORCE)
set(SHADERC_SKIP_EXECUTABLES        ON CACHE BOOL "" FORCE)
set(SHADERC_SKIP_INSTALL            ON CACHE BOOL "" FORCE)
set(BUILD_EXTERNAL                  OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK                     OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING                   OFF CACHE BOOL "" FORCE)
set(ENABLE_GLSLANG_BINARIES         OFF CACHE BOOL "" FORCE)
set(ENABLE_GLSLANG_INSTALL          OFF CACHE BOOL "" FORCE)
set(ENABLE_OPT                      ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST                   OFF CACHE BOOL "" FORCE)
set(GLSLANG_TESTS                   OFF CACHE BOOL "" FORCE)
set(SPIRV_SKIP_TESTS                ON CACHE BOOL "" FORCE)
set(SPIRV_SKIP_EXECUTABLES          ON CACHE BOOL "" FORCE)
set(SPIRV_HEADERS_ENABLE_INSTALL    OFF CACHE BOOL "" FORCE)
set(SPIRV_HEADERS_ENABLE_TESTS      OFF CACHE BOOL "" FORCE)
add_subdirectory("external/shaderc")

set_target_properties(shaderc shaderc_combined shaderc_combined-pkg-config shaderc_shared shaderc_static-pkg-config shaderc_util shaderc-pkg-config testdata PROPERTIES FOLDER "_external_shaderc")
set_target_properties(add-copyright build-version core_tables extinst_tables PROPERTIES FOLDER "_external_shaderc")
set_target_properties(spirv-tools-check-python-types spirv-tools-check-python-tests spirv-tools-vimsyntax SPIRV-Tools-diff SPIRV-Tools-link SPIRV-Tools-lint SPIRV-Tools-opt SPIRV-Tools-reduce SPIRV-Tools-shared SPIRV-Tools-static PROPERTIES FOLDER "_external_SPIR-V")
set_target_properties(spirv-tools-pkg-config spirv-tools-tables spirv-tools-build-version spirv-tools-header-DebugInfo spirv-tools-header-NonSemanticShaderDebugInfo100 spirv-tools-header-OpenCLDebugInfo100 PROPERTIES FOLDER "_external_SPIR-V")
set_target_properties(GenericCodeGen glslang glslang-default-resource-limits MachineIndependent OSDependent SPIRV SPVRemapper glslc PROPERTIES FOLDER "_external_glslang")

add_subdirectory("src")