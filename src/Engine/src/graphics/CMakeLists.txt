add_subdirectory("vk")
add_subdirectory("registry")
add_subdirectory("pass")
add_subdirectory("descriptors")

SET(SOURCES
	gpu_types.hpp
        VulkanContext.hpp
        VulkanContext.cpp
        Graphics.hpp
        Graphics.cpp
        Presenter.hpp
        Presenter.cpp
        ColorAttachment.hpp
        ColorAttachment.cpp
        FrameHandler.hpp
        FrameHandler.cpp
        Renderer.hpp
        Renderer.cpp
        TransferManager.hpp
        TransferManager.cpp
	DepthAttachment.hpp
	DepthAttachment.cpp
)

#set(EXPORT_HEADERS
#)

target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})
#copy_export_headers("${EXPORT_HEADERS}" "${INCLUDE_DIR}/graphics")