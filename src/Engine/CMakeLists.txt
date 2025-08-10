include(${PROJECT_SOURCE_DIR}/cmake/clangformat.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/CompilerWarnings.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/cppcheck.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/pch.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/ProjectSettings.cmake)

SET(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/engine")

add_library(${ENGINE_TARGET} STATIC)
add_subdirectory("src")
get_target_property(ENGINE_TARGET_SOURCES ${ENGINE_TARGET} SOURCES)
source_group(TREE "${PROJECT_SOURCE_DIR}/.." FILES ${ENGINE_TARGET_SOURCES})

use_pch(${ENGINE_TARGET})
apply_compiler_warnings(${ENGINE_TARGET})
update_output_directory(${ENGINE_TARGET})
enforce_cxx_standard(${ENGINE_TARGET})

enable_cppcheck(${ENGINE_TARGET} "${CMAKE_CURRENT_SOURCE_DIR}/src")
enable_clang_format(${ENGINE_TARGET} "${CMAKE_CURRENT_SOURCE_DIR}/src")
enable_enum_switch_error(${ENGINE_TARGET})

target_include_directories(${ENGINE_TARGET} PRIVATE "${PROJECT_SOURCE_DIR}/common")
target_include_directories(${ENGINE_TARGET} PRIVATE "external")
target_compile_definitions(${ENGINE_TARGET} PRIVATE LOG_DIRECTORY="/logs/engine")

include_glm(${ASSET_LOADER_PROJECT} "${CMAKE_CURRENT_SOURCE_DIR}/external")

add_dependencies(${ENGINE_TARGET} ${DEBUGGING_PROJECT})
target_link_libraries(${ENGINE_TARGET} PRIVATE ${DEBUGGING_PROJECT})
target_include_directories(${ENGINE_TARGET} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../Debugging/include")

add_dependencies(${ENGINE_TARGET} SDL3::SDL3)
target_link_libraries(${ENGINE_TARGET} PRIVATE SDL3::SDL3)
target_include_directories(${ENGINE_TARGET} PRIVATE "${PROJECT_SOURCE_DIR}/external/sdl/include")

set_target_properties(${ENGINE_TARGET} PROPERTIES FOLDER "Engine")