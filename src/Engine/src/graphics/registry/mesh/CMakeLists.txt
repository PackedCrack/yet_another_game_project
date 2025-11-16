
SET(SOURCES
	MeshRegistry.hpp
	MeshRegistry.cpp
	MeshID.hpp
)

set(EXPORT_HEADERS
	MeshID.hpp
)

target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})
copy_export_headers("${EXPORT_HEADERS}" "${INCLUDE_DIR}/graphics/registry/mesh")