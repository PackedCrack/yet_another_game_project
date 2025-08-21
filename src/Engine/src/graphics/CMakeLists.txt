add_subdirectory("window")
add_subdirectory("vk")

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
        TransferManager.hpp
        TransferManager.cpp
        MeshRegistry.hpp
        MeshRegistry.cpp
)

set(EXPORT_HEADERS
	Graphics.hpp
)

target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})
copy_export_headers("${EXPORT_HEADERS}" "${INCLUDE_DIR}/graphics")