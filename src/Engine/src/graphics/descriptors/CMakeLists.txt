SET(SOURCES
	DescriptorSet.hpp
	Global.hpp
	Global.cpp
	Indirect.hpp
	Indirect.cpp
)

target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})