SET(SOURCES
        Logger.hpp
        Logger.cpp
        debug_defines.hpp
)

set(EXPORT_HEADERS
        Logger.hpp
        debug_defines.hpp
)

target_sources(${DEBUGGING_PROJECT} PRIVATE ${SOURCES})
copy_export_headers("${EXPORT_HEADERS}" ${INCLUDE_DIR})