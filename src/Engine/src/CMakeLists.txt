add_subdirectory("components")
add_subdirectory("graphics")
add_subdirectory("state")
add_subdirectory("window")


SET(SOURCES
        engine_defines.hpp
        OdinInfo.hpp
        Odin.hpp
        Odin.cpp
	FilepathResolver.hpp
	FilepathResolver.cpp
	ArenaAllocator.hpp
	BumpAllocator.hpp
	ECS.hpp
	ECS.cpp
	Entity.hpp
	Entity.cpp
        ICamera.hpp
	InputEvent.hpp
	Core.hpp
	Core.cpp
	Input.hpp
	Input.cpp
)

set(EXPORT_HEADERS
        Odin.hpp
	OdinInfo.hpp
	ECS.hpp
	Entity.hpp
	ICamera.hpp
	InputEvent.hpp
	Core.hpp
)

target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})
copy_export_headers("${EXPORT_HEADERS}" ${INCLUDE_DIR})