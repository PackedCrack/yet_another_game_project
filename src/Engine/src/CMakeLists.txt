add_subdirectory("graphics")

SET(SOURCES
        engine_defines.hpp
        OdinInfo.hpp
        Odin.hpp
        Odin.cpp
        MeshRegistry.hpp
        MeshRegistry.cpp
	ArenaAllocator.hpp
	BumpAllocator.hpp
        ICamera.hpp
)

set(EXPORT_HEADERS
        Odin.hpp
	OdinInfo.hpp
	ICamera.hpp
)

target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})
copy_export_headers("${EXPORT_HEADERS}" ${INCLUDE_DIR})