#include "VulkanContext.hpp"
//
//
namespace
{
[[nodiscard]] VkApplicationInfo make_application_info(std::string_view name)
{
    return VkApplicationInfo{ .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                              .pNext = nullptr,
                              .pApplicationName = name.data(),
                              .applicationVersion = VK_MAKE_VERSION(1, 33, 7),
                              .pEngineName = "Odin",
                              .engineVersion = VK_MAKE_VERSION(1, 0, 0),
                              .apiVersion = VK_API_VERSION_1_4 };
}
}    // namespace
namespace odin::graphics
{
VulkanContext::VulkanContext(const OdinInfo& info, const window::Window& window)
    : m_Instance{ make_application_info(info.applicationName), window.required_extensions() }
    , m_DebugMsg{ m_Instance }
    , m_Device{}
    , m_PhysicalDevice{}
{}
}    // namespace odin::graphics
