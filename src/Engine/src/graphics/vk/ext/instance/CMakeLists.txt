SET(SOURCES
	get_instance_procedure.hpp
	debug_utils.hpp
	debug_utils.cpp
)


target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})