add_subdirectory("vk")
add_subdirectory("registry")

SET(SOURCES
	gpu_types.hpp
        VulkanContext.hpp
        VulkanContext.cpp
        Graphics.hpp
        Graphics.cpp
        Presenter.hpp
        Presenter.cpp
        FrameHandler.hpp
        FrameHandler.cpp
        Renderer.hpp
        Renderer.cpp
        ColorAttachment.hpp
        ColorAttachment.cpp
        ForwardPass.hpp
        ForwardPass.cpp
        TransferManager.hpp
        TransferManager.cpp
)

#set(EXPORT_HEADERS
#)

target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})
#copy_export_headers("${EXPORT_HEADERS}" "${INCLUDE_DIR}/graphics")