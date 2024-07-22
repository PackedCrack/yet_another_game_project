SET(SOURCES
        LoggerImpl.h
        LoggerImpl.cpp
        Logger.h
        Logger.cpp
        debug_defines.h
)

set(EXPORT_HEADERS
        Logger.h
        debug_defines.h
)

target_sources(${DEBUGGING_PROJECT} PRIVATE ${SOURCES})
copy_export_headers("${EXPORT_HEADERS}" ${INCLUDE_DIR})