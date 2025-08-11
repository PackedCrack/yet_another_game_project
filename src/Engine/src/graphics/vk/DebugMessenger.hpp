#pragma once

#include "Instance.hpp"
#include "ext/DebugUtilsMessengerProcs.hpp"
// vulkan
#include <vulkan/vulkan.h>
//
//
namespace odin::graphics::vk
{
class DebugMessenger
{
public:
    explicit DebugMessenger(const Instance& instance);
    ~DebugMessenger();
    DebugMessenger(const DebugMessenger& other) = delete;
    DebugMessenger(DebugMessenger&& other) noexcept;
    DebugMessenger& operator=(const DebugMessenger& other) = delete;
    DebugMessenger& operator=(DebugMessenger&& other) noexcept;

    [[nodiscard]] VkDebugUtilsMessengerEXT handle() const;
private:
    [[nodiscard]] VkDebugUtilsMessengerEXT create_debug_messenger() const;
private:
    ext::DebugUtilsMessengerProcs m_Procedures;
    VkDebugUtilsMessengerEXT m_Messenger = VK_NULL_HANDLE;
};
}    // namespace odin::graphics::vk
