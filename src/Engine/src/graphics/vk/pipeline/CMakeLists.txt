SET(SOURCES
	PipelineBuilder.hpp
	PipelineBuilder.cpp
	PipelineLayout.hpp
	PipelineLayout.cpp
	Pipeline.hpp
	GraphicsPipeline.hpp
	GraphicsPipeline.cpp
	ComputePipeline.hpp
	ComputePipeline.cpp
	DescriptorSetLayout.hpp
	DescriptorSetLayout.cpp
	DescriptorSetLayoutBuilder.hpp
	DescriptorSetLayoutBuilder.cpp
	DescriptorPool.hpp
	DescriptorPool.cpp
	DescriptorSetAllocator.hpp
	DescriptorSetAllocator.cpp
	DescriptorWriter.hpp
	DescriptorWriter.cpp
)


target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})