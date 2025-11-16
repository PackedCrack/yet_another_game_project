SET(SOURCES
	get_device_procedure.hpp
	extended_dynamic_state3.hpp
	extended_dynamic_state3.cpp
)


target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})