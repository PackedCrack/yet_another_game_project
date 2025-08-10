include(${PROJECT_SOURCE_DIR}/cmake/clangformat.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/CompilerWarnings.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/cppcheck.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/pch.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/ProjectSettings.cmake)


add_executable(${GAME_PROJECT})
add_subdirectory("src")
get_target_property(GAME_PROJECT_SOURCES ${GAME_PROJECT} SOURCES)
source_group(TREE "${PROJECT_SOURCE_DIR}/.." FILES ${GAME_PROJECT_SOURCES})

use_pch(${GAME_PROJECT})
apply_compiler_warnings(${GAME_PROJECT})
update_output_directory(${GAME_PROJECT})
enforce_cxx_standard(${GAME_PROJECT})

enable_cppcheck(${GAME_PROJECT} "${CMAKE_CURRENT_SOURCE_DIR}/src")
enable_clang_format(${GAME_PROJECT} "${CMAKE_CURRENT_SOURCE_DIR}/src")
enable_enum_switch_error(${GAME_PROJECT})

add_dependencies(${GAME_PROJECT} ${ASSET_LOADER_PROJECT})
target_link_libraries(${GAME_PROJECT} PRIVATE ${ASSET_LOADER_PROJECT})
target_include_directories(${GAME_PROJECT} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../AssetLoader/include")


add_dependencies(${GAME_PROJECT} ${DEBUGGING_PROJECT})
target_link_libraries(${GAME_PROJECT} PRIVATE ${DEBUGGING_PROJECT})
target_include_directories(${GAME_PROJECT} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../Debugging/include")
