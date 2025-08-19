SET(SOURCES
	PipelineLayout.hpp
	PipelineLayout.cpp
	Pipeline.hpp
	GraphicsPipeline.hpp
	GraphicsPipeline.cpp
	ComputePipeline.hpp
	ComputePipeline.cpp
	DescriptorSetLayout.hpp
	DescriptorSetLayout.cpp
	DescriptorPool.hpp
	DescriptorPool.cpp
)


target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})