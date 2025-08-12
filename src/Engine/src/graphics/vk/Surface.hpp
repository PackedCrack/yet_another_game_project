#pragma once

#include "../window/Window.hpp"
#include "Instance.hpp"
// vulkan
#include <vulkan/vulkan.h>
//
//
namespace odin::graphics::vk
{
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
    [[nodiscard]] VkSurfaceKHR handle() const;
    [[nodiscard]] bool queue_family_supports_present(VkPhysicalDevice device, std::uint32_t index) const;
private:
    VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
    InstanceView m_Instance;
};
}    // namespace odin::graphics::vk
