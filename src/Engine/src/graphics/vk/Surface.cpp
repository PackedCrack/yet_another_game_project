#include "Surface.hpp"

#include "vulkan_defines.hpp"
// sdl
#include <SDL3/SDL_vulkan.h>
//
//
namespace odin::graphics::vk
{
Surface::Surface(window::Window& wnd, const Instance& instance)
    : m_Surface{ wnd.make_surface(instance) }
    , m_Instance{ instance.handle() }
{}
Surface::~Surface()
{
    if (m_Surface != VK_NULL_HANDLE)
    {
        SDL_Vulkan_DestroySurface(m_Instance.handle, m_Surface, nullptr);
    }
}
Surface::Surface(Surface&& other) noexcept
    : m_Surface{ std::exchange(other.m_Surface, m_Surface) }
    , m_Instance{ std::exchange(other.m_Instance, m_Instance) }
{}
Surface& Surface::operator=(Surface&& other) noexcept
{
    if (this != std::addressof(other))
    {
        m_Surface = std::exchange(other.m_Surface, m_Surface);
        m_Instance = std::exchange(other.m_Instance, m_Instance);
    }

    return *this;
}
VkSurfaceKHR Surface::handle() const
{
    ODIN_ASSERT(m_Surface != VK_NULL_HANDLE);
    return m_Surface;
}
bool Surface::queue_family_supports_present(VkPhysicalDevice device, std::uint32_t queueFamilyIndex) const
{
    VkBool32 surfaceSupport{};
    VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(device, queueFamilyIndex, m_Surface, &surfaceSupport),
             "Failed to get Physical Device Surface Support for Queue Family: {}.",
             queueFamilyIndex);

    return surfaceSupport;
}
}    // namespace odin::graphics::vk
