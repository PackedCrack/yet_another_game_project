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