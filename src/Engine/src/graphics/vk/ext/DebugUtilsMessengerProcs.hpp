#pragma once

#include "InstanceProcLoader.hpp"
//
//
namespace odin::graphics::vk::ext
{
class DebugUtilsMessengerProcs : public InstanceProcLoader<DebugUtilsMessengerProcs>
{
    using PFN_create_messenger = PFN_vkCreateDebugUtilsMessengerEXT;
    using PFN_destroy_messenger = PFN_vkDestroyDebugUtilsMessengerEXT;
public:
    DebugUtilsMessengerProcs(const Instance& instance);
public:
    [[nodiscard]] VkDebugUtilsMessengerEXT create_debug_utils_messenger(VkDebugUtilsMessengerCreateInfoEXT info) const;
    void destroy_debug_utils_messenger(VkDebugUtilsMessengerEXT messenger) const;
private:
    std::reference_wrapper<const Instance> m_Instance;
    PFN_create_messenger create_messenger;
    PFN_destroy_messenger destroy_messenger;
};
}    // namespace odin::graphics::vk::ext
