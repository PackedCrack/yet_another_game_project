SET(SOURCES
	ImageRegistry.hpp
	ImageRegistry.cpp
	ImageHandle.hpp
	ImageHandle.cpp
	ImageSlot.hpp
)


target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})