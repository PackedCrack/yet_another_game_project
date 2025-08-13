add_subdirectory("window")
add_subdirectory("vk")

SET(SOURCES
        VulkanContext.hpp
        VulkanContext.cpp
        Graphics.hpp
        Graphics.cpp

)

set(EXPORT_HEADERS
	Graphics.hpp
)

target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})
copy_export_headers("${EXPORT_HEADERS}" "${INCLUDE_DIR}/graphics")