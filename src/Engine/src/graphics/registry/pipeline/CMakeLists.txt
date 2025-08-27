SET(SOURCES
	PipelineRegistry.hpp
	PipelineRegistry.cpp
	RequestBuilder.hpp
	RequestBuilder.cpp
	Request.hpp
	GraphicsHandle.hpp
	GraphicsHandle.cpp
	GraphicsSlot.hpp
	PipelineKey.hpp
	DescriptorSetLayoutKey.hpp
	PipelineLayoutKey.hpp
)


target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})