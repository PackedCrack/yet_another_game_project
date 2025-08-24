include(${PROJECT_SOURCE_DIR}/cmake/clangformat.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/CompilerWarnings.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/cppcheck.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/pch.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/ProjectSettings.cmake)

SET(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/debug")
file(REMOVE_RECURSE "${INCLUDE_DIR}")
file(MAKE_DIRECTORY "${INCLUDE_DIR}")

add_library(${DEBUGGING_PROJECT} STATIC)
add_subdirectory("src")
get_target_property(DEBUGGING_PROJECT_SOURCES ${DEBUGGING_PROJECT} SOURCES)
source_group(TREE "${PROJECT_SOURCE_DIR}/.." FILES ${DEBUGGING_PROJECT_SOURCES})

target_compile_definitions(${DEBUGGING_PROJECT} PRIVATE SELF_COMPILIATION)
if (MSVC)
    target_compile_options(${DEBUGGING_PROJECT} PRIVATE "/utf-8")
endif ()

use_pch(${DEBUGGING_PROJECT})
apply_compiler_warnings(${DEBUGGING_PROJECT})
update_output_directory(${DEBUGGING_PROJECT})
enforce_cxx_standard(${DEBUGGING_PROJECT})

#enable_cppcheck(${DEBUGGING_PROJECT} "${CMAKE_CURRENT_SOURCE_DIR}/src")
enable_clang_format(${DEBUGGING_PROJECT} "${CMAKE_CURRENT_SOURCE_DIR}/src")
enable_enum_switch_error(${DEBUGGING_PROJECT})

target_include_directories(${DEBUGGING_PROJECT} PRIVATE "${PROJECT_SOURCE_DIR}/common")
target_include_directories(${DEBUGGING_PROJECT} PRIVATE "external")


# Add cppcheck dependency manually because this project requires passing additional custom cmd line arguments to the script
find_program(PYTHON python)
if(PYTHON)
    add_custom_target(${DEBUGGING_PROJECT}_cppcheck_dependency
            COMMAND python cppcheck.py "${CMAKE_CURRENT_SOURCE_DIR}/src" "-DSELF_COMPILATION"
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            COMMENT "Running CppCheck.."
            VERBATIM
    )
else()
    message(WARNING "Could not find the environment variable for \"python\".. Trying python3..")
    find_program(PYTHON3 python3)
    if(PYTHON3)
        add_custom_target(${DEBUGGING_PROJECT}_cppcheck_dependency
                COMMAND python3 cppcheck.py "${CMAKE_CURRENT_SOURCE_DIR}/src" "-DSELF_COMPILATION"
                WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                COMMENT "Running CppCheck.."
                VERBATIM
        )
    else()
        message(ERROR "Could not find the environment variable for \"python3\". Python is required to build with development configuration.")
    endif()
endif()

add_dependencies(${DEBUGGING_PROJECT} ${DEBUGGING_PROJECT}_cppcheck_dependency)

set_target_properties(${DEBUGGING_PROJECT}
                      ${DEBUGGING_PROJECT}_cppcheck_dependency    
                      PROPERTIES 
                      FOLDER 
                      "Debug")