SET(SOURCES
        Forward.hpp
        Forward.cpp
	IndirectSetup.hpp
	IndirectSetup.cpp
	FrustumCull.hpp
	FrustumCull.cpp
	InstanceCompaction.hpp
	InstanceCompaction.cpp
)

target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})