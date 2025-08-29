SET(SOURCES
	ShaderRegistry.hpp
	ShaderRegistry.cpp
	ShaderHandle.hpp
	ShaderHandle.cpp
	ShaderSlot.hpp
)


target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})