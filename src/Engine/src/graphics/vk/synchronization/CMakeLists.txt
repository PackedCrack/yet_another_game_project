SET(SOURCES
	Semaphore.hpp
	Semaphore.cpp
	Fence.hpp
	Fence.cpp
)


target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})