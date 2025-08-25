SET(SOURCES
	Input.hpp
)

set(EXPORT_HEADERS
	Input.hpp
)

target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})
copy_export_headers("${EXPORT_HEADERS}" ${INCLUDE_DIR}/state)