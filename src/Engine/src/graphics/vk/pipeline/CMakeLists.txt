SET(SOURCES
	PipelineLayout.hpp
	PipelineLayout.cpp
	Pipeline.hpp
	GraphicsPipeline.hpp
	GraphicsPipeline.cpp
	ComputePipeline.hpp
	ComputePipeline.cpp
)


target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})