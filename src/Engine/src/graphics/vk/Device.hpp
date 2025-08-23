//
// Created by qwerty on 12/08/2025.
//
#pragma once
//
//
#include "Instance.hpp"
#include "PhysicalDevice.hpp"
//
//
namespace odin::graphics::vk
{
class QueueFamilies;
struct DeviceRef
{
    VkDevice handle;
};
class Device
{
public:
    Device(const PhysicalDevice& phyDevice, QueueFamilies& queueFamilies);
    ~Device();
    Device(const Device& other) = delete;
    Device(Device&& other) noexcept;
    Device& operator=(const Device& other) = delete;
    Device& operator=(Device&& other) noexcept;
public:
    [[nodiscard]] DeviceRef handle() const;
    [[nodiscard]] VkQueue get_queue_handle(std::uint32_t familyIndex) const;
private:
    VkDevice m_Device = VK_NULL_HANDLE;
};
}    // namespace odin::graphics::vk
