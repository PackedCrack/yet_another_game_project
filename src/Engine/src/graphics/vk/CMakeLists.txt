add_subdirectory("ext")
add_subdirectory("resource")
add_subdirectory("synchronization")

SET(SOURCES
        vulkan_defines.hpp
        vulkan_info.hpp
        Allocator.hpp
        Allocator.cpp
	DebugMessenger.hpp
	DebugMessenger.cpp
        Device.hpp
        Device.cpp
        Instance.hpp
        Instance.cpp
        PhysicalDevice.hpp
        PhysicalDevice.cpp
	PhysicalDeviceFeatures.hpp
	PhysicalDeviceFeatures.cpp
	PhysicalDeviceProperties.hpp
	PhysicalDeviceProperties.cpp
	QueueFamilies.hpp
	QueueFamilies.cpp
	Surface.hpp
        Surface.cpp
        Swapchain.hpp
        Swapchain.cpp
        CommandBuffer.hpp
        CommandBuffer.cpp
        CommandPool.hpp
        CommandPool.cpp
)


target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})