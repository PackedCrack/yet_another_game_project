add_subdirectory("device")
add_subdirectory("instance")

SET(SOURCES
	device_extensions.hpp
	device_extensions.cpp
	instance_extensions.hpp
	instance_extensions.cpp
)


target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})