//
// Created by qwerty on 11/08/2025.
//
#pragma once

#include "PhysicalDeviceProperties.hpp"
#include "PhysicalDeviceFeatures.hpp"
#include "Instance.hpp"
#include "Surface.hpp"
//
//
namespace odin::graphics::vk
{
struct PhysicalDeviceRef
{
    VkPhysicalDevice handle;
};
class PhysicalDevice
{
public:
    [[nodiscard]] static const PhysicalDeviceProperties& properties();
    [[nodiscard]] static const PhysicalDeviceFeatures& features();
public:
    PhysicalDevice(const Instance& instance, const Surface& surface);
    ~PhysicalDevice() = default;
    PhysicalDevice(const PhysicalDevice& other);
    PhysicalDevice(PhysicalDevice&& other) noexcept;
    PhysicalDevice& operator=(const PhysicalDevice& other);
    PhysicalDevice& operator=(PhysicalDevice&& other) noexcept;
public:
    [[nodiscard]] PhysicalDeviceRef handle() const;
    [[nodiscard]] const std::vector<VkQueueFamilyProperties>& queue_families_properties() const;
private:
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    std::vector<VkQueueFamilyProperties> m_QueueProperties;
};
}    // namespace odin::graphics::vk
