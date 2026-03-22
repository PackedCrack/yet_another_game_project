//
// Created by qwerty on 12/08/2025.
//
#pragma once

#include "../../window/Window.hpp"
#include "Instance.hpp"
// vulkan
#include <vulkan/vulkan.h>
//
//
namespace odin::graphics::vk
{
class PhysicalDevice;
struct PhysicalDeviceRef;
struct SurfaceRef
{
    VkSurfaceKHR handle;
};
class Surface
{
public:
    Surface(window::Window& wnd, InstanceRef instance);
    ~Surface();
    Surface(const Surface& other) = delete;
    Surface(Surface&& other) noexcept;
    Surface& operator=(const Surface& other) = delete;
    Surface& operator=(Surface&& other) noexcept;
public:
    [[nodiscard]] SurfaceRef handle() const;
    [[nodiscard]] bool queue_family_supports_present(VkPhysicalDevice device, std::uint32_t index) const;
    [[nodiscard]] std::vector<VkPresentModeKHR> present_modes(PhysicalDeviceRef physicalDevice) const;
    [[nodiscard]] std::vector<VkSurfaceFormatKHR> available_formats(PhysicalDeviceRef physicalDevice) const;
    [[nodiscard]] std::uint32_t min_image_count(PhysicalDeviceRef physicalDevice);
    [[nodiscard]] std::uint32_t max_image_count(PhysicalDeviceRef physicalDevice);
    [[nodiscard]] VkSurfaceTransformFlagBitsKHR current_transform(PhysicalDeviceRef physicalDevice);
    [[nodiscard]] VkExtent2D current_extent(PhysicalDeviceRef physicalDevice);
private:
    [[nodiscard]] VkSurfaceCapabilitiesKHR get_surface_capabilities(PhysicalDeviceRef physicalDevice);
private:
    VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
    InstanceRef m_Instance;
};
}    // namespace odin::graphics::vk
