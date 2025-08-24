SET(SOURCES
        InstanceProcLoader.hpp
	DebugUtilsMessengerProcs.hpp
	DebugUtilsMessengerProcs.cpp
)


target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})