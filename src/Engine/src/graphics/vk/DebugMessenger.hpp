//
// Created by qwerty on 11/08/2025.
//
#pragma once

#include "Instance.hpp"
#include "ext/instance/debug_utils.hpp"
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

    //[[nodiscard]] VkDebugUtilsMessengerEXT handle() const;
private:
    [[nodiscard]] VkDebugUtilsMessengerEXT create_debug_messenger() const;
private:
    InstanceRef m_Instance;
    VkDebugUtilsMessengerEXT m_Messenger = VK_NULL_HANDLE;
};
}    // namespace odin::graphics::vk
