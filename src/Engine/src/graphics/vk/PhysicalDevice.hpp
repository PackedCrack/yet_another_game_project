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
//struct QueueFamilyIndices
//{
//    std::optional<uint32_t> graphics;
//    std::optional<uint32_t> present;
//    std::optional<uint32_t> compute;
//    std::optional<uint32_t> transfer;
//};
class PhysicalDevice
{
public:
    PhysicalDevice(const Instance& instance, const Surface& surface);
    ~PhysicalDevice() = default;
    PhysicalDevice(const PhysicalDevice& other);
    PhysicalDevice(PhysicalDevice&& other) noexcept;
    PhysicalDevice& operator=(const PhysicalDevice& other);
    PhysicalDevice& operator=(PhysicalDevice&& other) noexcept;

    [[nodiscard]] VkPhysicalDevice handle() const;
    /*[[nodiscard]] uint32_t queue_index_graphics() const;
    [[nodiscard]] uint32_t queue_index_present() const;
    [[nodiscard]] uint32_t queue_index_compute() const;
    [[nodiscard]] uint32_t queue_index_transfer() const;*/
    [[nodiscard]] const PhysicalDeviceProperties& properties() const;
    [[nodiscard]] const PhysicalDeviceFeatures& features() const;
    [[nodiscard]] const std::vector<VkQueueFamilyProperties>& queue_families_properties() const;
private:
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    PhysicalDeviceProperties m_Properties;
    PhysicalDeviceFeatures m_Features;
    std::vector<VkQueueFamilyProperties> m_QueueProperties;
    //VkPhysicalDeviceProperties2 m_Properties;
    //VkPhysicalDeviceVulkan11Properties m_11Properties;
    //VkPhysicalDeviceVulkan12Properties m_12Properties;
    //VkPhysicalDeviceFeatures2 m_Features;
    //VkPhysicalDeviceVulkan11Features m_11Features;
    //VkPhysicalDeviceVulkan12Features m_12Features;

    //QueueFamilyIndices m_QueueFamilies;
};
}    // namespace odin::graphics::vk
