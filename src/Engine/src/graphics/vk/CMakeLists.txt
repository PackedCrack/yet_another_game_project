add_subdirectory("ext")

SET(SOURCES
        vulkan_defines.hpp
        vulkan_info.hpp
	DebugMessenger.hpp
	DebugMessenger.cpp
        Device.hpp
        Device.cpp
        Instance.hpp
        Instance.cpp
	Surface.hpp
        Surface.cpp
        PhysicalDevice.hpp
        PhysicalDevice.cpp
	PhysicalDeviceProperties.hpp
	PhysicalDeviceProperties.cpp
	PhysicalDeviceFeatures.hpp
	PhysicalDeviceFeatures.cpp
	QueueFamilies.hpp
	QueueFamilies.cpp
	Image.hpp
	Image.cpp
)


target_sources(${ENGINE_TARGET} PRIVATE ${SOURCES})