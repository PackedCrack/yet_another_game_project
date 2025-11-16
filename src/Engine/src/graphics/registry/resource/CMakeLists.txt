add_subdirectory("shader")
add_subdirectory("buffer")

SET(SOURCES
	ResourceRegistry.hpp
	ResourceRegistry.cpp
)


target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})