include(${PROJECT_SOURCE_DIR}/cmake/clangformat.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/CompilerWarnings.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/cppcheck.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/pch.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/ProjectSettings.cmake)


add_executable(${GAME_TARGET})
add_subdirectory("src")
get_target_property(GAME_TARGET_SOURCES ${GAME_TARGET} SOURCES)
source_group(TREE "${PROJECT_SOURCE_DIR}/.." FILES ${GAME_TARGET_SOURCES})

use_pch(${GAME_TARGET})
apply_compiler_warnings(${GAME_TARGET})
update_output_directory(${GAME_TARGET})
enforce_cxx_standard(${GAME_TARGET})

enable_cppcheck(${GAME_TARGET} "${CMAKE_CURRENT_SOURCE_DIR}/src")
enable_clang_format(${GAME_TARGET} "${CMAKE_CURRENT_SOURCE_DIR}/src")
enable_enum_switch_error(${GAME_TARGET})

include_glm(${GAME_TARGET} "${CMAKE_CURRENT_SOURCE_DIR}/external")

target_include_directories(${GAME_TARGET} PRIVATE "external")
target_include_directories(${GAME_TARGET} PRIVATE "${PROJECT_SOURCE_DIR}/common")
target_compile_definitions(${GAME_TARGET} PRIVATE LOG_DIRECTORY="/logs/game")

add_dependencies(${GAME_TARGET} ${ASSET_LOADER_PROJECT})
target_link_libraries(${GAME_TARGET} PRIVATE ${ASSET_LOADER_PROJECT})
target_include_directories(${GAME_TARGET} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../AssetLoader/include")

add_dependencies(${GAME_TARGET} ${DEBUGGING_PROJECT})
target_link_libraries(${GAME_TARGET} PRIVATE ${DEBUGGING_PROJECT})
target_include_directories(${GAME_TARGET} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../Debugging/include")

add_dependencies(${GAME_TARGET} ${ENGINE_TARGET})
target_link_libraries(${GAME_TARGET} PRIVATE ${ENGINE_TARGET})
target_include_directories(${GAME_TARGET} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../Engine/include")

target_include_directories(${GAME_TARGET} PRIVATE "${PROJECT_SOURCE_DIR}/external/entt/single_include")

# TODO: Find a better way to link to vulkan.
find_package(Vulkan REQUIRED)
target_link_libraries(${GAME_TARGET} PRIVATE "$ENV{VULKAN_SDK}/Lib/vulkan-1.lib")