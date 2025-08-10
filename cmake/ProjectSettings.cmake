function(update_output_directory PROJECT)
    set_target_properties(${PROJECT} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY}")
    set_target_properties(${PROJECT} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}")
    set_target_properties(${PROJECT} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}")

    set_target_properties(${PROJECT} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${RUNTIME_OUTPUT_DIRECTORY}")
    set_target_properties(${PROJECT} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG "${RUNTIME_OUTPUT_DIRECTORY}")
    set_target_properties(${PROJECT} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE "${RUNTIME_OUTPUT_DIRECTORY}")

    set_target_properties(${PROJECT} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${LIB_OUTPUT_DIR}")
    set_target_properties(${PROJECT} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${LIB_OUTPUT_DIR}")
    set_target_properties(${PROJECT} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${LIB_OUTPUT_DIR}")

    set_target_properties(${PROJECT} PROPERTIES OUTPUT_NAME_DEBUG "${PROJECT}d")
    set_target_properties(${PROJECT} PROPERTIES OUTPUT_NAME_RELEASE "${PROJECT}")
endfunction()

function(enforce_cxx_standard PROJECT)
    set_target_properties(${PROJECT} PROPERTIES CXX_STANDARD 23)
    set_target_properties(${PROJECT} PROPERTIES CXX_STANDARD_REQUIRED ON)
endfunction()

function(copy_export_headers HEADERS INCLUDE_DIRECTORY)
    foreach(HEADER ${HEADERS})
        get_filename_component(FILENAME ${HEADER} NAME)
        configure_file(${HEADER} ${INCLUDE_DIRECTORY}/${FILENAME} COPYONLY)
    endforeach()
endfunction()

function(include_glm PROJECT DESTINATION)
    file(REMOVE_RECURSE "${DESTINATION}/glm")
    file(MAKE_DIRECTORY "${DESTINATION}/glm")
    file(COPY "${PROJECT_SOURCE_DIR}/external/glm/glm" DESTINATION "${DESTINATION}")

    target_compile_definitions(${PROJECT} PRIVATE GLM_ENABLE_EXPERIMENTAL GLM_FORCE_DEPTH_ZERO_TO_ONE GLM_FORCE_XYZW_ONLY GLM_FORCE_QUAT_DATA_XYZW GLM_FORCE_QUAT_CTOR_XYZW)
endfunction()

function(enable_enum_switch_error PROJECT)
    if (MSVC)
        # Missing enum cases, etc.
        target_compile_options(${PROJECT} PRIVATE /we4061 /we4062 /we4065)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(${PROJECT} PRIVATE -Werror=switch)
    endif ()
endfunction()