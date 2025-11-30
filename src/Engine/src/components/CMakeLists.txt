SET(SOURCES
	LocalTRS.hpp
	WorldTRS.hpp
	Geometry.hpp
	Parent.hpp
	Model.hpp
	Model.cpp
	DirtyFlag.hpp
	Children.hpp
)

set(EXPORT_HEADERS
	LocalTRS.hpp
	WorldTRS.hpp
	Geometry.hpp
	Parent.hpp
	Model.hpp
	DirtyFlag.hpp
	Children.hpp
)

target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})
copy_export_headers("${EXPORT_HEADERS}" ${INCLUDE_DIR}/components)