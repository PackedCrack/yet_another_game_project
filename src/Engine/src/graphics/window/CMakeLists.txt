SET(SOURCES
        sdl_defines.hpp
        Window.hpp
        Window.cpp
)

set(EXPORT_HEADERS
        Window.hpp
)

target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})
copy_export_headers("${EXPORT_HEADERS}" ${INCLUDE_DIR})