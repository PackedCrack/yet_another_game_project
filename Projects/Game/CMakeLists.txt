include(${PROJECT_SOURCE_DIR}/cmake/clangformat.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/CompilerWarnings.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/cppcheck.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/ProjectSettings.cmake)


add_executable(${GAME_PROJECT})
add_subdirectory("src")
get_target_property(GAME_PROJECT_SOURCES ${GAME_PROJECT} SOURCES)
source_group(TREE "${PROJECT_SOURCE_DIR}/.." FILES ${GAME_PROJECT_SOURCES})


apply_compiler_warnings(${GAME_PROJECT})
update_output_directory(${GAME_PROJECT})
enforce_cxx_standard(${GAME_PROJECT})

enable_cppcheck(${GAME_PROJECT} ${CMAKE_CURRENT_SOURCE_DIR})
enable_clang_format(${GAME_PROJECT} ${CMAKE_CURRENT_SOURCE_DIR})
