add_subdirectory("shader")
add_subdirectory("buffer")
add_subdirectory("image")

SET(SOURCES
	ResourceRegistry.hpp
	ResourceRegistry.cpp
)


target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})