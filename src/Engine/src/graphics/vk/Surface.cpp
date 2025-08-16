//
// Created by qwerty on 12/08/2025.
//
#include "Surface.hpp"

#include "PhysicalDevice.hpp"
#include "vulkan_defines.hpp"
// sdl
#include <SDL3/SDL_vulkan.h>
//
//
namespace odin::graphics::vk
{
Surface::Surface(window::Window& wnd, InstanceRef instance)
    : m_Surface{ wnd.make_surface(instance) }
    , m_Instance{ instance }
    , m_Capabilities{ std::nullopt }
{}
Surface::~Surface()
{
    if (m_Surface != VK_NULL_HANDLE)
    {
        SDL_Vulkan_DestroySurface(m_Instance.handle, m_Surface, nullptr);
    }
}
Surface::Surface(Surface&& other) noexcept
    : m_Surface{ VK_NULL_HANDLE }
    , m_Instance{}
{
    std::swap(m_Surface, other.m_Surface);
    m_Instance = other.m_Instance;
}
Surface& Surface::operator=(Surface&& other) noexcept
{
    if (this != std::addressof(other))
    {
        m_Surface = std::exchange(other.m_Surface, m_Surface);
        m_Instance = std::exchange(other.m_Instance, m_Instance);
    }

    return *this;
}
SurfaceRef Surface::handle() const
{
    ODIN_ASSERT(m_Surface != VK_NULL_HANDLE);
    return SurfaceRef{ .handle = m_Surface };
}
bool Surface::queue_family_supports_present(VkPhysicalDevice device, std::uint32_t queueFamilyIndex) const
{
    VkBool32 surfaceSupport{};
    VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(device, queueFamilyIndex, m_Surface, &surfaceSupport),
             "Failed to get Physical Device Surface Support for Queue Family: {}.",
             queueFamilyIndex);

    return surfaceSupport;
}
std::vector<VkPresentModeKHR> Surface::present_modes(PhysicalDeviceRef physicalDevice) const
{
    std::uint32_t count{};
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice.handle, m_Surface, std::addressof(count), nullptr),
             "Failed to get Vulkan Surface Present Mode count.");
    std::vector<VkPresentModeKHR> modes(count);
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice.handle, m_Surface, std::addressof(count), modes.data()),
             "Failed to get Vulkan Surface Present Mode(s)");

    return modes;
}
std::vector<VkSurfaceFormatKHR> Surface::available_formats(PhysicalDeviceRef physicalDevice) const
{
    std::uint32_t count{};
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice.handle, m_Surface, std::addressof(count), nullptr),
             "Failed to get Vulkan Surface Format(s) count.");
    std::vector<VkSurfaceFormatKHR> formats(count);
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice.handle, m_Surface, std::addressof(count), formats.data()),
             "Failed to get Vulkan Surface Format(s).");

    return formats;
}
[[nodiscard]] std::uint32_t Surface::min_image_count(PhysicalDeviceRef physicalDevice)
{
    const VkSurfaceCapabilitiesKHR& capabilities = get_surface_capabilities(physicalDevice);
    return capabilities.minImageCount;
}
[[nodiscard]] std::uint32_t Surface::max_image_count(PhysicalDeviceRef physicalDevice)
{
    const VkSurfaceCapabilitiesKHR& capabilities = get_surface_capabilities(physicalDevice);
    return capabilities.maxImageCount;
}
VkSurfaceTransformFlagBitsKHR Surface::current_transform(PhysicalDeviceRef physicalDevice)
{
    const VkSurfaceCapabilitiesKHR& capabilities = get_surface_capabilities(physicalDevice);
    return capabilities.currentTransform;
}
VkExtent2D Surface::current_extent(PhysicalDeviceRef physicalDevice)
{
    const VkSurfaceCapabilitiesKHR& capabilities = get_surface_capabilities(physicalDevice);
    return capabilities.currentExtent;
}
const VkSurfaceCapabilitiesKHR& Surface::get_surface_capabilities(PhysicalDeviceRef physicalDevice)
{
    ODIN_ASSERT(m_Surface != VK_NULL_HANDLE);
    if (m_Capabilities)
    {
        return m_Capabilities.value();
    }

    m_Capabilities = std::make_optional<VkSurfaceCapabilitiesKHR>();
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice.handle, m_Surface, std::addressof(m_Capabilities.value())),
             "Failed to get Vulkan Surface Capabilities");

    return m_Capabilities.value();
}
}    // namespace odin::graphics::vk
