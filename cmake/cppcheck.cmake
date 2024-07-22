function(enable_cppcheck PROJECT SOURCE_DIR)
    find_program(PYTHON python)

    if(PYTHON)
        add_custom_target(${PROJECT}_cppcheck_dependency
                COMMAND python cppcheck.py ${SOURCE_DIR}
                WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                COMMENT "Running CppCheck.."
                VERBATIM
        )
    else()
        message(WARNING "Could not find the environment variable for \"python\".. Trying python3..")
        find_program(PYTHON3 python3)
        if(PYTHON3)
            add_custom_target(${PROJECT}_cppcheck_dependency
                    COMMAND python3 cppcheck.py ${SOURCE_DIR}
                    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                    COMMENT "Running CppCheck.."
                    VERBATIM
            )
        else()
            message(ERROR "Could not find the environment variable for \"python3\". Python is required to build with development configuration.")
        endif()
    endif()

    add_dependencies(${PROJECT} ${PROJECT}_cppcheck_dependency)
endfunction()