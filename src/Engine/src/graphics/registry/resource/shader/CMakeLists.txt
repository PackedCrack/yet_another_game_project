SET(SOURCES
	ShaderRegistry.hpp
	ShaderRegistry.cpp
	ShaderCompiler.hpp
	ShaderCompiler.cpp
	ShaderHandle.hpp
	ShaderHandle.cpp
	ShaderSlot.hpp
)


target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})