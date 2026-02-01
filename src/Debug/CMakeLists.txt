include(${PROJECT_SOURCE_DIR}/cmake/clangformat.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/CompilerWarnings.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/cppcheck.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/pch.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/ProjectSettings.cmake)

SET(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/debug")
file(REMOVE_RECURSE "${INCLUDE_DIR}")
file(MAKE_DIRECTORY "${INCLUDE_DIR}")

add_library(${DEBUG_TARGET} STATIC)
add_subdirectory("src")
get_target_property(DEBUG_TARGET_SOURCES ${DEBUG_TARGET} SOURCES)
source_group(TREE "${PROJECT_SOURCE_DIR}/.." FILES ${DEBUG_TARGET_SOURCES})

target_compile_definitions(${DEBUG_TARGET} PRIVATE SELF_COMPILIATION)
if (MSVC)
    target_compile_options(${DEBUG_TARGET} PRIVATE "/utf-8")
endif ()

use_pch(${DEBUG_TARGET})
apply_compiler_warnings(${DEBUG_TARGET})
update_output_directory(${DEBUG_TARGET})
enforce_cxx_standard(${DEBUG_TARGET})

#enable_cppcheck(${DEBUG_TARGET} "${CMAKE_CURRENT_SOURCE_DIR}/src")
enable_clang_format(${DEBUG_TARGET} "${CMAKE_CURRENT_SOURCE_DIR}/src")
enable_enum_switch_error(${DEBUG_TARGET})
include_spdlog(${DEBUG_TARGET} "${CMAKE_CURRENT_SOURCE_DIR}/external")

target_include_directories(${DEBUG_TARGET} PRIVATE "${PROJECT_SOURCE_DIR}/common")
target_include_directories(${DEBUG_TARGET} PRIVATE "external")


# Add cppcheck dependency manually because this project requires passing additional custom cmd line arguments to the script
find_program(PYTHON python)
if(PYTHON)
    add_custom_target(${DEBUG_TARGET}_cppcheck_dependency
            COMMAND python cppcheck.py "${CMAKE_CURRENT_SOURCE_DIR}/src" "-DSELF_COMPILATION"
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            COMMENT "Running CppCheck.."
            VERBATIM
    )
else()
    message(WARNING "Could not find the environment variable for \"python\".. Trying python3..")
    find_program(PYTHON3 python3)
    if(PYTHON3)
        add_custom_target(${DEBUG_TARGET}_cppcheck_dependency
                COMMAND python3 cppcheck.py "${CMAKE_CURRENT_SOURCE_DIR}/src" "-DSELF_COMPILATION"
                WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                COMMENT "Running CppCheck.."
                VERBATIM
        )
    else()
        message(ERROR "Could not find the environment variable for \"python3\". Python is required to build with development configuration.")
    endif()
endif()

add_dependencies(${DEBUG_TARGET} ${DEBUG_TARGET}_cppcheck_dependency)


set_target_properties(${DEBUG_TARGET} PROPERTIES FOLDER "Engine")
set_target_properties("${DEBUG_TARGET}_cppcheck_dependency" PROPERTIES FOLDER "Build Dependencies")
set_target_properties("${DEBUG_TARGET}_format_dependency" PROPERTIES FOLDER "Build Dependencies")