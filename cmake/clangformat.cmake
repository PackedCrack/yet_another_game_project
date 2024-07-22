function(enable_clang_format PROJECT SOURCE_ROOT_DIR)
    find_program(CLANG_FORMAT clang-format)

    if(CLANG_FORMAT)
        file(GLOB_RECURSE ALL_SOURCE_FILES
                "SOURCE_ROOT_DIR/*.cpp"
                "SOURCE_ROOT_DIR/*.hpp"
                "SOURCE_ROOT_DIR/*.h"
        )

        add_custom_target(
                ${PROJECT}_format_dependency
                COMMAND clang-format -i -style=file ${ALL_SOURCE_FILES}
                COMMENT "Running clang-format on all source files."
        )
    else()
        message(ERROR "Could not find the environment variable for \"clang-format\". clang-format is required to build with development configuration.")
    endif()

    add_dependencies(${PROJECT} ${PROJECT}_format_dependency)
endfunction()