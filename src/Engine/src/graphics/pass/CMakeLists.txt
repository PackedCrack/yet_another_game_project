SET(SOURCES
        Forward.hpp
        Forward.cpp
	IndirectSetup.hpp
	IndirectSetup.cpp
)

target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})