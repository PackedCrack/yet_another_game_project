SET(SOURCES
	LocalTRS.hpp
	WorldTRS.hpp
	Mesh.hpp
	Parent.hpp
	Model.hpp
)

set(EXPORT_HEADERS
	LocalTRS.hpp
	WorldTRS.hpp
	Mesh.hpp
	Parent.hpp
	Model.hpp
)

target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})
copy_export_headers("${EXPORT_HEADERS}" ${INCLUDE_DIR}/components)