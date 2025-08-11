#pragma once


// vulkan
#include <vulkan/vulkan.h>
//
//
namespace odin::graphics::vk
{
class Instance
{
public:
    explicit Instance(VkApplicationInfo appInfo, const std::vector<std::string_view>& windowExtensions);
    ~Instance();
    Instance(const Instance& other) = delete;
    Instance(Instance&& other) noexcept;
    Instance& operator=(const Instance& other) = delete;
    Instance& operator=(Instance&& other) noexcept;

    [[nodiscard]] VkInstance handle() const;
private:
    VkInstance m_Instance = VK_NULL_HANDLE;
    //VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
};
}    // namespace odin::graphics::vk
