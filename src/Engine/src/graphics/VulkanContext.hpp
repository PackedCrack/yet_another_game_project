//
// Created by qwerty on 11/08/2025.
//
#pragma once

#include "vk/Allocator.hpp"
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
    VulkanContext(vk::Instance instance,
                  vk::PhysicalDevice physicalDevice,
                  vk::QueueFamilies queueFamilies,
                  vk::Device device,
                  std::shared_ptr<vk::Allocator> pAllocator);
public:
    [[nodiscard]] const vk::Instance& instance() const;
    [[nodiscard]] const vk::PhysicalDevice& physical_device() const;
    [[nodiscard]] const vk::QueueFamilies& queue_families() const;
    [[nodiscard]] const vk::Device& device() const;
    [[nodiscard]] std::shared_ptr<vk::Allocator> allocator() const;
private:
    vk::Instance m_Instance;
    std::optional<vk::DebugMessenger> m_DebugMsg;
    vk::PhysicalDevice m_PhysicalDevice;
    vk::QueueFamilies m_Queues;
    vk::Device m_Device;
    std::shared_ptr<vk::Allocator> m_pAllocator = nullptr;
};
}    // namespace odin::graphics
