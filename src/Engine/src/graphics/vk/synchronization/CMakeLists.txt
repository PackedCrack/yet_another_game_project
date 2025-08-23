SET(SOURCES
	Semaphore.hpp
	Semaphore.cpp
	TimelineSemaphore.hpp
	TimelineSemaphore.cpp
	Fence.hpp
	Fence.cpp
)


target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})