SET(SOURCES
	Image.hpp
	Image.cpp
	ImageView.hpp
	ImageView.cpp
)


target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})