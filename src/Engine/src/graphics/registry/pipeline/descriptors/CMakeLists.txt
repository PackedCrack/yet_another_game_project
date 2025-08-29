SET(SOURCES
	DescriptorSetLayoutRegistry.hpp
	DescriptorSetLayoutRegistry.cpp
	DescriptorSetLayoutKey.hpp
)


target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})