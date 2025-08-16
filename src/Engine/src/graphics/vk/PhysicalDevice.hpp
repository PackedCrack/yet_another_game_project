//
// Created by qwerty on 11/08/2025.
//
#pragma once

#include "PhysicalDeviceProperties.hpp"
#include "PhysicalDeviceFeatures.hpp"
#include "Instance.hpp"
#include "Surface.hpp"
#include "../window/Window.hpp"
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
    PhysicalDevice(const Instance& instance, const Surface& surface);
    ~PhysicalDevice() = default;
    PhysicalDevice(const PhysicalDevice& other);
    PhysicalDevice(PhysicalDevice&& other) noexcept;
    PhysicalDevice& operator=(const PhysicalDevice& other);
    PhysicalDevice& operator=(PhysicalDevice&& other) noexcept;

    [[nodiscard]] PhysicalDeviceRef handle() const;
    [[nodiscard]] const PhysicalDeviceProperties& properties() const;
    [[nodiscard]] const PhysicalDeviceFeatures& features() const;
    [[nodiscard]] const std::vector<VkQueueFamilyProperties>& queue_families_properties() const;
private:
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    PhysicalDeviceProperties m_Properties;
    PhysicalDeviceFeatures m_Features;
    std::vector<VkQueueFamilyProperties> m_QueueProperties;
};
}    // namespace odin::graphics::vk
