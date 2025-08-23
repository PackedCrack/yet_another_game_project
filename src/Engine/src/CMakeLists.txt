add_subdirectory("graphics")
add_subdirectory("components")

SET(SOURCES
        engine_defines.hpp
        OdinInfo.hpp
        Odin.hpp
        Odin.cpp
	ArenaAllocator.hpp
	BumpAllocator.hpp
	ECS.hpp
	ECS.cpp
	Entity.hpp
	Entity.cpp
        ICamera.hpp
)

set(EXPORT_HEADERS
        Odin.hpp
	OdinInfo.hpp
	ECS.hpp
	Entity.hpp
	ICamera.hpp
)

target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})
copy_export_headers("${EXPORT_HEADERS}" ${INCLUDE_DIR})