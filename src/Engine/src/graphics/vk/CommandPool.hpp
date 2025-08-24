//
// Created by qwerty on 15/08/2025.
//
#pragma once

#include "CommandBuffer.hpp"
#include "Device.hpp"
#include "QueueFamilies.hpp"
//
//
namespace odin::graphics::vk
{
struct CommandPoolRef
{
    VkCommandPool handle;
};
class CommandPool
{
public:
    CommandPool(DeviceRef device, QueueView queueFamily);
    ~CommandPool();
    CommandPool(const CommandPool& other) = delete;
    CommandPool(CommandPool&& other) noexcept;
    CommandPool& operator=(const CommandPool& other) = delete;
    CommandPool& operator=(CommandPool&& other) noexcept;
public:
    [[nodiscard]] CommandPoolRef handle() const;
    [[nodiscard]] CommandBuffer make_command_buffer() const;
private:
    VkCommandPool m_CommandPool = VK_NULL_HANDLE;
    DeviceRef m_Device;
};
}    // namespace odin::graphics::vk
