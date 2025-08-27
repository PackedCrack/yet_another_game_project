add_subdirectory("mesh")
add_subdirectory("pipeline")
add_subdirectory("resource")

SET(SOURCES
	Handle.hpp
	Slot.hpp
)


target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})