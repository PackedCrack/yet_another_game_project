SET(SOURCES
	PipelineRegistry.hpp
	PipelineRegistry.cpp
	RequestBuilder.hpp
	RequestBuilder.cpp
	Request.hpp
	GraphicsHandle.hpp
	GraphicsHandle.cpp
	GraphicsSlot.hpp
	ComputeHandle.hpp
	ComputeHandle.cpp
	ComputeSlot.hpp
	PipelineKey.hpp
	PipelineLayoutRegistry.hpp
	PipelineLayoutRegistry.cpp
	PipelineLayoutKey.hpp
	PipelineHandle.hpp
	DescriptorSetLayoutRegistry.hpp
	DescriptorSetLayoutRegistry.cpp
	DescriptorSetLayoutKey.hpp
)


target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})