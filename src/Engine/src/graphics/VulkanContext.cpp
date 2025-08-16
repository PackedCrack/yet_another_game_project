//
// Created by qwerty on 11/08/2025.
//
#include "VulkanContext.hpp"
//
//
namespace
{}    // namespace
namespace odin::graphics
{
VulkanContext::VulkanContext(vk::Instance instance, vk::PhysicalDevice physicalDevice, vk::QueueFamilies queueFamilies, vk::Device device)
    : m_Instance{ std::move(instance) }
    , m_DebugMsg{ std::nullopt }
    , m_PhysicalDevice{ std::move(physicalDevice) }
    , m_Queues{ std::move(queueFamilies) }
    , m_Device{ std::move(device) }
{
#ifndef NDEBUG
    m_DebugMsg = std::make_optional<vk::DebugMessenger>(m_Instance);
#endif
}
const vk::Instance& VulkanContext::instance() const
{
    return m_Instance;
}
const vk::PhysicalDevice& VulkanContext::physical_device() const
{
    return m_PhysicalDevice;
}
const vk::QueueFamilies& VulkanContext::queue_families() const
{
    return m_Queues;
}
const vk::Device& VulkanContext::device() const
{
    return m_Device;
}
}    // namespace odin::graphics
