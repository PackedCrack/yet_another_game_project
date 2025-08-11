SET(SOURCES
        sdl_defines.hpp
        Window.hpp
        Window.cpp
        DisplayResolution.hpp
        DisplayResolution.cpp
)

set(EXPORT_HEADERS
	DisplayResolution.hpp
)

target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})
copy_export_headers("${EXPORT_HEADERS}" "${INCLUDE_DIR}/graphics/window")