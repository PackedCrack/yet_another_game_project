macro(fetch_third_party)
    # --- fetch content --- #
    SET(FETCHCONTENT_BASE_DIR "${CMAKE_BINARY_DIR}/_deps")
    SET(DEPENDENCY_BUILD_PATH "${CMAKE_BINARY_DIR}/dependencies/build")
    SET(DEPENDENCY_SRC_PATH "${CMAKE_BINARY_DIR}/dependencies/src")

    #include(cmake/openxr-sdk.cmake)
endmacro()