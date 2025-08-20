SET(SOURCES
	Image.hpp
	Image.cpp
	ImageView.hpp
	ImageView.cpp
	Buffer.hpp
	StagingBuffer.hpp
	StagingBuffer.cpp
	UniformBuffer.hpp
	UniformBuffer.cpp
	StorageBuffer.hpp
	StorageBuffer.cpp
	VertexBuffer.hpp
	VertexBuffer.cpp
)


target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})