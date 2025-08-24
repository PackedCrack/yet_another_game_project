include(${PROJECT_SOURCE_DIR}/cmake/clangformat.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/CompilerWarnings.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/cppcheck.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/pch.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/ProjectSettings.cmake)

SET(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/assetloader")
file(REMOVE_RECURSE "${INCLUDE_DIR}")
file(MAKE_DIRECTORY "${INCLUDE_DIR}")

add_library(${ASSET_LOADER_PROJECT} STATIC)
add_subdirectory("src")
get_target_property(ASSET_LOADER__SOURCES ${ASSET_LOADER_PROJECT} SOURCES)
source_group(TREE "${PROJECT_SOURCE_DIR}/.." FILES ${ASSET_LOADER__SOURCES})

use_pch(${ASSET_LOADER_PROJECT})
apply_compiler_warnings(${ASSET_LOADER_PROJECT})
update_output_directory(${ASSET_LOADER_PROJECT})
enforce_cxx_standard(${ASSET_LOADER_PROJECT})

enable_cppcheck(${ASSET_LOADER_PROJECT} "${CMAKE_CURRENT_SOURCE_DIR}/src")
enable_clang_format(${ASSET_LOADER_PROJECT} "${CMAKE_CURRENT_SOURCE_DIR}/src")
enable_enum_switch_error(${ASSET_LOADER_PROJECT})

target_include_directories(${ASSET_LOADER_PROJECT} PRIVATE "${PROJECT_SOURCE_DIR}/common")
target_include_directories(${ASSET_LOADER_PROJECT} PRIVATE "external")
target_compile_definitions(${ASSET_LOADER_PROJECT} PRIVATE LOG_DIRECTORY="/logs/assetloader")

include_glm(${ASSET_LOADER_PROJECT} "${CMAKE_CURRENT_SOURCE_DIR}/external")

file(REMOVE_RECURSE "${CMAKE_CURRENT_SOURCE_DIR}/external/spdlog")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/external/spdlog")
file(COPY "${PROJECT_SOURCE_DIR}/external/spdlog/include/spdlog" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/external")

file(REMOVE_RECURSE "${CMAKE_CURRENT_SOURCE_DIR}/external/tinygltf")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/external/tinygltf")
file(COPY "${PROJECT_SOURCE_DIR}/external/tinygltf/json.hpp" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/external/tinygltf")
file(COPY "${PROJECT_SOURCE_DIR}/external/tinygltf/stb_image.h" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/external/tinygltf")
file(COPY "${PROJECT_SOURCE_DIR}/external/tinygltf/stb_image_write.h" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/external/tinygltf")
file(COPY "${PROJECT_SOURCE_DIR}/external/tinygltf/tiny_gltf.cc" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/external/tinygltf")
file(COPY "${PROJECT_SOURCE_DIR}/external/tinygltf/tiny_gltf.h" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/external/tinygltf")

add_dependencies(${ASSET_LOADER_PROJECT} ${DEBUGGING_PROJECT})
target_link_libraries(${ASSET_LOADER_PROJECT} PRIVATE ${DEBUGGING_PROJECT})
target_include_directories(${ASSET_LOADER_PROJECT} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../Debugging/include")