SET(SOURCES
	Image.hpp
	Image.cpp
	ImageView.hpp
	ImageView.cpp
	Buffer.hpp
	DynamicBuffer.hpp
	StagingBuffer.hpp
	StagingBuffer.cpp
	UniformBuffer.hpp
	UniformBuffer.cpp
	DynamicUniformBuffer.hpp
	DynamicUniformBuffer.cpp
	DynamicStorageBuffer.hpp
	DynamicStorageBuffer.cpp
	StorageBuffer.hpp
	StorageBuffer.cpp
	VertexBuffer.hpp
	VertexBuffer.cpp
	IndexBuffer.hpp
	IndexBuffer.cpp
	ShaderModule.hpp
	ShaderModule.cpp
)


target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})