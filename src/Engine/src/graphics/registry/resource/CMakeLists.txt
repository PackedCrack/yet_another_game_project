add_subdirectory("shader")

SET(SOURCES
	ResourceRegistry.hpp
	ResourceRegistry.cpp
)


target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})