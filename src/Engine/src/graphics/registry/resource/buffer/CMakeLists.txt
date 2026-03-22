SET(SOURCES
	BufferRegistry.hpp
	BufferRegistry.cpp
	BufferHandle.hpp
	BindView.hpp
)


target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})