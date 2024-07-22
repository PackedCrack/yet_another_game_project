include(${PROJECT_SOURCE_DIR}/cmake/clangformat.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/CompilerWarnings.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/cppcheck.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/pch.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/ProjectSettings.cmake)

SET(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/debug")

add_library(${DEBUGGING_PROJECT} STATIC)
add_subdirectory("src")
get_target_property(DEBUGGING_PROJECT_SOURCES ${DEBUGGING_PROJECT} SOURCES)
source_group(TREE "${PROJECT_SOURCE_DIR}/.." FILES ${DEBUGGING_PROJECT_SOURCES})

target_compile_definitions(${DEBUGGING_PROJECT} PRIVATE SELF_COMPILIATION)

use_pch(${DEBUGGING_PROJECT})
apply_compiler_warnings(${DEBUGGING_PROJECT})
update_output_directory(${DEBUGGING_PROJECT})
enforce_cxx_standard(${DEBUGGING_PROJECT})

enable_cppcheck(${DEBUGGING_PROJECT} "${CMAKE_CURRENT_SOURCE_DIR}/src")
enable_clang_format(${DEBUGGING_PROJECT} "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_include_directories(${DEBUGGING_PROJECT} PRIVATE "${PROJECT_SOURCE_DIR}/common")
target_include_directories(${DEBUGGING_PROJECT} PRIVATE "external")
