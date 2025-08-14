#pragma once

#include "../window/Window.hpp"
#include "Instance.hpp"
// vulkan
#include <vulkan/vulkan.h>
//
//
namespace odin::graphics::vk
{
class PhysicalDevice;
struct SurfaceRef
{
    VkSurfaceKHR handle;
};
class Surface
{
public:
    Surface(window::Window& wnd, const Instance& instance);
    ~Surface();
    Surface(const Surface& other) = delete;
    Surface(Surface&& other) noexcept;
    Surface& operator=(const Surface& other) = delete;
    Surface& operator=(Surface&& other) noexcept;
public:
    [[nodiscard]] SurfaceRef handle() const;
    [[nodiscard]] bool queue_family_supports_present(VkPhysicalDevice device, std::uint32_t index) const;
    [[nodiscard]] std::vector<VkPresentModeKHR> present_modes(const PhysicalDevice& physicalDevice) const;
    [[nodiscard]] std::vector<VkSurfaceFormatKHR> available_formats(const PhysicalDevice& physicalDevice) const;
    [[nodiscard]] std::uint32_t min_image_count(const PhysicalDevice& physicalDevice);
    [[nodiscard]] std::uint32_t max_image_count(const PhysicalDevice& physicalDevice);
    [[nodiscard]] VkSurfaceTransformFlagBitsKHR current_transform(const PhysicalDevice& physicalDevice);
    [[nodiscard]] VkExtent2D current_extent(const PhysicalDevice& physicalDevice);
private:
    [[nodiscard]] const VkSurfaceCapabilitiesKHR& get_surface_capabilities(const PhysicalDevice& physicalDevice);
private:
    VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
    InstanceRef m_Instance;
    std::optional<VkSurfaceCapabilitiesKHR> m_Capabilities;
};
}    // namespace odin::graphics::vk
