#pragma once

#include "vk/DebugMessenger.hpp"
#include "vk/Device.hpp"
#include "vk/Instance.hpp"
#include "vk/PhysicalDevice.hpp"
#include "vk/QueueFamilies.hpp"
//
//
namespace odin::graphics
{
class VulkanContext
{
public:
    VulkanContext(vk::Instance instance, vk::PhysicalDevice physicalDevice, vk::QueueFamilies queueFamilies, vk::Device device);
public:
    [[nodiscard]] const vk::Instance& instance() const;
    [[nodiscard]] const vk::PhysicalDevice& physical_device() const;
    [[nodiscard]] const vk::QueueFamilies& queue_families() const;
    [[nodiscard]] const vk::Device& device() const;
private:
    vk::Instance m_Instance;
    std::optional<vk::DebugMessenger> m_DebugMsg;
    vk::PhysicalDevice m_PhysicalDevice;
    vk::QueueFamilies m_Queues;
    vk::Device m_Device;
};
}    // namespace odin::graphics
