//
// Created by qwerty on 11/08/2025.
//
#pragma once

#include <vulkan/vulkan.h>
//
//
namespace odin::graphics::vk
{
class PhysicalDeviceProperties
{
public:
    PhysicalDeviceProperties() = default;
    PhysicalDeviceProperties(VkPhysicalDevice gpu);
    ~PhysicalDeviceProperties() = default;
    PhysicalDeviceProperties(const PhysicalDeviceProperties& other);
    PhysicalDeviceProperties(PhysicalDeviceProperties&& other) noexcept;
    PhysicalDeviceProperties& operator=(const PhysicalDeviceProperties& other);
    PhysicalDeviceProperties& operator=(PhysicalDeviceProperties&& other) noexcept;
public:
    [[nodiscard]] VkPhysicalDeviceType device_type() const;
    [[nodiscard]] std::string device_name() const;
    [[nodiscard]] std::string driver_name() const;
    [[nodiscard]] std::string driver_info() const;
    [[nodiscard]] std::string api_version() const;
    [[nodiscard]] std::size_t min_memory_map_alignment() const;
    [[nodiscard]] VkDeviceSize min_uniform_buffer_offset_alignment() const;
    [[nodiscard]] VkDeviceSize min_storage_buffer_offset_alignment() const;
    [[nodiscard]] std::uint32_t max_compute_work_group_invocations() const;
    [[nodiscard]] std::uint32_t max_compute_work_group_count_x() const;
    [[nodiscard]] std::uint32_t max_compute_work_group_count_y() const;
    [[nodiscard]] std::uint32_t max_compute_work_group_count_z() const;
    [[nodiscard]] std::uint32_t max_work_group_size_x() const;
    [[nodiscard]] std::uint32_t max_work_group_size_y() const;
    [[nodiscard]] std::uint32_t max_work_group_size_z() const;
private:
    void update_linked_list();
private:
    VkPhysicalDeviceVulkan14Properties m_v14Properties;
    VkPhysicalDeviceVulkan13Properties m_v13Properties;
    VkPhysicalDeviceVulkan12Properties m_v12Properties;
    VkPhysicalDeviceVulkan11Properties m_v11Properties;
    VkPhysicalDeviceProperties2 m_Properties;
};
}    // namespace odin::graphics::vk
