//
// Created by qwerty on 11/08/2025.
//
#include "PhysicalDeviceProperties.hpp"

#include "vulkan_defines.hpp"
#include "vulkan_info.hpp"
//
//
namespace odin::graphics::vk
{
PhysicalDeviceProperties::PhysicalDeviceProperties(VkPhysicalDevice gpu)
    : m_v14Properties{ physical_device_properies_v14() }
    , m_v13Properties{ physical_device_properies_v13(std::addressof(m_v14Properties)) }
    , m_v12Properties{ physical_device_properies_v12(std::addressof(m_v13Properties)) }
    , m_v11Properties{ physical_device_properies_v11(std::addressof(m_v12Properties)) }
    , m_Properties{ physical_device_properies_2(std::addressof(m_v11Properties)) }
{
    ODIN_ASSERT(gpu != VK_NULL_HANDLE);

    vkGetPhysicalDeviceProperties2(gpu, std::addressof(m_Properties));

    std::string logMsg = std::format("{} Properties:\n", device_name().c_str());
    logMsg += "\nDriver Name: " + driver_name();
    logMsg += "\nDriver Version: " + driver_info();
    logMsg += "\nVulkan API version: " + api_version();
    logMsg += "\nMinimum Memory map alignment: " + std::to_string(min_memory_map_alignment());
    logMsg += "\nMinimum Uniform buffer offset alignment: " + std::to_string(min_uniform_buffer_offset_alignment());
    logMsg += "\nMinimum Storage buffer offset alignment: " + std::to_string(min_storage_buffer_offset_alignment());
    logMsg += "\nMaximum Compute Work Group Total Invocations: " + std::to_string(max_compute_work_group_invocations());
    logMsg += "\nMaximum Compute Work Group Size - X: " + std::to_string(work_group_size_x());
    logMsg += "\nMaximum Compute Work Group Size - Y: " + std::to_string(work_group_size_y());
    logMsg += "\nMaximum Compute Work Group Size - Z: " + std::to_string(work_group_size_z());
    LOG_INFO(logMsg);
}
PhysicalDeviceProperties::PhysicalDeviceProperties(const PhysicalDeviceProperties& other)
    : m_Properties{ other.m_Properties }
    , m_v11Properties{ other.m_v11Properties }
    , m_v12Properties{ other.m_v12Properties }
    , m_v13Properties{ other.m_v13Properties }
    , m_v14Properties{ other.m_v14Properties }
{
    update_linked_list();
}
PhysicalDeviceProperties::PhysicalDeviceProperties(PhysicalDeviceProperties&& other) noexcept
    : m_Properties{ std::move(other.m_Properties) }
    , m_v11Properties{ std::move(other.m_v11Properties) }
    , m_v12Properties{ std::move(other.m_v12Properties) }
    , m_v13Properties{ std::move(other.m_v13Properties) }
    , m_v14Properties{ std::move(other.m_v14Properties) }
{
    update_linked_list();
}
PhysicalDeviceProperties& PhysicalDeviceProperties::operator=(const PhysicalDeviceProperties& other)
{
    if (this != std::addressof(other))
    {
        m_Properties = other.m_Properties;
        m_v11Properties = other.m_v11Properties;
        m_v12Properties = other.m_v12Properties;
        m_v13Properties = other.m_v13Properties;
        m_v14Properties = other.m_v14Properties;

        update_linked_list();
    }

    return *this;
}
PhysicalDeviceProperties& PhysicalDeviceProperties::operator=(PhysicalDeviceProperties&& other) noexcept
{
    if (this != std::addressof(other))
    {
        m_Properties = std::move(other.m_Properties);
        m_v11Properties = std::move(other.m_v11Properties);
        m_v12Properties = std::move(other.m_v12Properties);
        m_v13Properties = std::move(other.m_v13Properties);
        m_v14Properties = std::move(other.m_v14Properties);

        update_linked_list();
    }

    return *this;
}
VkPhysicalDeviceType PhysicalDeviceProperties::device_type() const
{
    return m_Properties.properties.deviceType;
}
std::string PhysicalDeviceProperties::device_name() const
{
    return m_Properties.properties.deviceName;
}
std::string PhysicalDeviceProperties::driver_name() const
{
    return m_v12Properties.driverName;
}
std::string PhysicalDeviceProperties::driver_info() const
{
    return m_v12Properties.driverInfo;
}
std::string PhysicalDeviceProperties::api_version() const
{
    std::uint32_t apiVersion = m_Properties.properties.apiVersion;
    std::string version = std::to_string(VK_API_VERSION_MAJOR(apiVersion));
    version += ".";
    version += std::to_string(VK_API_VERSION_MINOR(apiVersion));
    version += ".";
    version += std::to_string(VK_API_VERSION_PATCH(apiVersion));
    return version;
}
std::size_t PhysicalDeviceProperties::min_memory_map_alignment() const
{
    return m_Properties.properties.limits.minMemoryMapAlignment;
}
VkDeviceSize PhysicalDeviceProperties::min_uniform_buffer_offset_alignment() const
{
    return m_Properties.properties.limits.minUniformBufferOffsetAlignment;
}
VkDeviceSize PhysicalDeviceProperties::min_storage_buffer_offset_alignment() const
{
    return m_Properties.properties.limits.minStorageBufferOffsetAlignment;
}
std::uint32_t PhysicalDeviceProperties::max_compute_work_group_invocations() const
{
    return m_Properties.properties.limits.maxComputeWorkGroupInvocations;
}
std::uint32_t PhysicalDeviceProperties::work_group_size_x() const
{
    return m_Properties.properties.limits.maxComputeWorkGroupSize[0u];
}
std::uint32_t PhysicalDeviceProperties::work_group_size_y() const
{
    return m_Properties.properties.limits.maxComputeWorkGroupSize[1u];
}
std::uint32_t PhysicalDeviceProperties::work_group_size_z() const
{
    return m_Properties.properties.limits.maxComputeWorkGroupSize[2u];
}
void PhysicalDeviceProperties::update_linked_list()
{
    m_Properties.pNext = std::addressof(m_v11Properties);
    m_v11Properties.pNext = std::addressof(m_v12Properties);
    m_v12Properties.pNext = std::addressof(m_v13Properties);
    m_v13Properties.pNext = std::addressof(m_v14Properties);
}
}    // namespace odin::graphics::vk
