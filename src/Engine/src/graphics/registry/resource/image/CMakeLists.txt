SET(SOURCES
	ImageRegistry.hpp
	ImageRegistry.cpp
	AttachmentRegistry.hpp
	AttachmentRegistry.cpp
)


target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})