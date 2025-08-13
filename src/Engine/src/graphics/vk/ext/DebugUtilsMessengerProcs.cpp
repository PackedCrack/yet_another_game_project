#include "DebugUtilsMessengerProcs.hpp"

#include "../vulkan_defines.hpp"
//
//
namespace odin::graphics::vk::ext
{
DebugUtilsMessengerProcs::DebugUtilsMessengerProcs(const Instance& instance)
    : m_Instance{ instance.handle() }
    , create_messenger{ get_procedure_address<PFN_create_messenger>(m_Instance, "vkCreateDebugUtilsMessengerEXT") }
    , destroy_messenger{ get_procedure_address<PFN_destroy_messenger>(m_Instance, "vkDestroyDebugUtilsMessengerEXT") }
{}
[[nodiscard]] VkDebugUtilsMessengerEXT DebugUtilsMessengerProcs::create_debug_utils_messenger(VkDebugUtilsMessengerCreateInfoEXT info) const
{
    VkDebugUtilsMessengerEXT messenger{};
    VK_CHECK(create_messenger(m_Instance.handle, std::addressof(info), nullptr, std::addressof(messenger)),
             "Failed to create Debug Messenger.");

    return messenger;
}
void DebugUtilsMessengerProcs::destroy_debug_utils_messenger(VkDebugUtilsMessengerEXT messenger) const
{
    destroy_messenger(m_Instance.handle, messenger, nullptr);
}
}    // namespace odin::graphics::vk::ext
