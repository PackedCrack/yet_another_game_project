//
// Created by qwerty on 12/08/2025.
//
#include "QueueFamilies.hpp"

#include "Device.hpp"
#include "vulkan_defines.hpp"
#include "vulkan_info.hpp"
//
//
namespace
{
using namespace odin::graphics::vk;

using index_t = QueueFamilies::index_t;
[[nodiscard]] bool family_supports_graphics(const VkQueueFamilyProperties& property)
{
    if (property.queueFlags & VK_QUEUE_GRAPHICS_BIT)
    {
        return true;
    }

    return false;
}
[[nodiscard]] bool family_supports_compute(const VkQueueFamilyProperties& property)
{
    if (property.queueFlags & VK_QUEUE_COMPUTE_BIT)
    {
        return true;
    }

    return false;
}
[[nodiscard]] bool family_supports_transfer(const VkQueueFamilyProperties& property)
{
    if (property.queueFlags & VK_QUEUE_TRANSFER_BIT)
    {
        return true;
    }

    return false;
}
index_t select_present(const PhysicalDevice& physDevice, const Surface& surface)
{
    const std::vector<VkQueueFamilyProperties>& properties = physDevice.queue_families_properties();

    std::optional<index_t> bestCandidate{};
    std::optional<index_t> backup{};
    for (std::size_t i = 0; i < properties.size(); ++i)
    {
        const VkQueueFamilyProperties& property = properties[i];

        PhysicalDeviceRef physDev = physDevice.handle();
        auto index = static_cast<index_t>(i);
        if (surface.queue_family_supports_present(physDev.handle, index))
        {
            if (!backup.has_value())
            {
                backup = std::make_optional(index);
            }

            if (family_supports_graphics(property) && family_supports_compute(property))
            {
                bestCandidate = std::make_optional(index);
            }
        }
    }
    if (!bestCandidate && !backup)
    {
        LOG_FATAL("Failed to find a potential Present queue.");
    }

    ODIN_ASSERT(backup);
    return bestCandidate.has_value() ? bestCandidate.value() : backup.value();
};
index_t select_graphics(const PhysicalDevice& physDevice, const Surface& surface)
{
    const std::vector<VkQueueFamilyProperties>& properties = physDevice.queue_families_properties();

    std::optional<index_t> bestCandidate{};
    std::optional<index_t> backup{};
    for (std::size_t i = 0; i < properties.size(); ++i)
    {
        const VkQueueFamilyProperties& property = properties[i];
        auto index = static_cast<index_t>(i);
        if (family_supports_graphics(property))
        {
            if (!backup.has_value())
            {
                backup = std::make_optional(index);
            }

            PhysicalDeviceRef physDev = physDevice.handle();
            if (family_supports_compute(property) && surface.queue_family_supports_present(physDev.handle, index))
            {
                bestCandidate = std::make_optional(index);
            }
        }
    }
    if (!bestCandidate && !backup)
    {
        LOG_FATAL("Failed to find a potential Graphics queue.");
    }

    ODIN_ASSERT(backup);
    return bestCandidate.has_value() ? bestCandidate.value() : backup.value();
};
index_t select_compute(const PhysicalDevice& physDevice, const Surface& surface)
{
    const std::vector<VkQueueFamilyProperties>& properties = physDevice.queue_families_properties();

    std::optional<index_t> bestCandidate{};
    std::optional<index_t> backup{};
    for (std::size_t i = 0; i < properties.size(); ++i)
    {
        const VkQueueFamilyProperties& property = properties[i];
        auto index = static_cast<index_t>(i);
        if (family_supports_compute(property))
        {
            if (!backup.has_value())
            {
                backup = std::make_optional(index);
            }

            PhysicalDeviceRef physDev = physDevice.handle();
            if (family_supports_graphics(property) && surface.queue_family_supports_present(physDev.handle, index))
            {
                bestCandidate = std::make_optional(index);
            }
        }
    }
    if (!bestCandidate && !backup)
    {
        LOG_FATAL("Failed to find a potential Compute queue.");
    }

    ODIN_ASSERT(backup);
    return bestCandidate.has_value() ? bestCandidate.value() : backup.value();
};
index_t select_transfer(const PhysicalDevice& physDevice, const Surface& surface)
{
    const std::vector<VkQueueFamilyProperties>& properties = physDevice.queue_families_properties();

    std::optional<index_t> bestCandidate{};
    std::optional<index_t> backup{};
    for (std::size_t i = 0; i < properties.size(); ++i)
    {
        const VkQueueFamilyProperties& property = properties[i];
        auto index = static_cast<index_t>(i);
        if (family_supports_transfer(property))
        {
            if (!backup.has_value())
            {
                backup = std::make_optional(index);
            }

            PhysicalDeviceRef physDev = physDevice.handle();
            if (!family_supports_graphics(property) && !surface.queue_family_supports_present(physDev.handle, index) &&
                !family_supports_compute(property))
            {
                bestCandidate = std::make_optional(index);
            }
        }
    }
    if (!bestCandidate && !backup)
    {
        LOG_FATAL("Failed to find a potential Compute queue.");
    }

    ODIN_ASSERT(backup);
    return bestCandidate.has_value() ? bestCandidate.value() : backup.value();
};
void log_selected_queue_families(QueueFamilies::Queue present,
                                 QueueFamilies::Queue graphics,
                                 QueueFamilies::Queue compute,
                                 QueueFamilies::Queue transfer)
{
    std::string msg = "\n\tSelected Families:";
    msg += std::format("\n\t\tPresent: {}", present.index);
    msg += std::format("\n\t\tGraphics: {}", graphics.index);
    msg += std::format("\n\t\tCompute: {}", compute.index);
    msg += std::format("\n\t\tTransfer: {}", transfer.index);

    LOG_INFO(msg);
}
}    // namespace
namespace odin::graphics::vk
{
QueueFamilies::QueueFamilies(const PhysicalDevice& device, const Surface& surface)
    : m_Present{ VK_NULL_HANDLE, INVALID_INDEX }
    , m_Graphics{ VK_NULL_HANDLE, INVALID_INDEX }
    , m_Compute{ VK_NULL_HANDLE, INVALID_INDEX }
    , m_Transfer{ VK_NULL_HANDLE, INVALID_INDEX }
{
    select_queue_indices(device, surface);
    log_selected_queue_families(m_Present, m_Graphics, m_Compute, m_Transfer);
}
QueueView QueueFamilies::present() const
{
    ODIN_ASSERT(m_Present.handle != VK_NULL_HANDLE);
    return QueueView{ m_Present.handle, m_Present.index };
}
QueueView QueueFamilies::graphics() const
{
    ODIN_ASSERT(m_Graphics.handle != VK_NULL_HANDLE);
    return QueueView{ m_Graphics.handle, m_Graphics.index };
}
QueueView QueueFamilies::compute() const
{
    ODIN_ASSERT(m_Compute.handle != VK_NULL_HANDLE);
    return QueueView{ m_Compute.handle, m_Compute.index };
}
QueueView QueueFamilies::transfer() const
{
    ODIN_ASSERT(m_Transfer.handle != VK_NULL_HANDLE);
    return QueueView{ m_Transfer.handle, m_Transfer.index };
}
std::vector<VkDeviceQueueCreateInfo> QueueFamilies::queue_create_info() const
{
    // Just hard set all queue prio to 1.. Not sure if it matters
    static constexpr float priority = 1.0f;
    std::uint32_t count = 1;
    std::vector<VkDeviceQueueCreateInfo> createInfos{ device_queue_create_info(m_Graphics.index, count, std::addressof(priority)) };
    if (m_Graphics.index != m_Present.index)
    {
        ++count;
        createInfos.emplace_back(device_queue_create_info(m_Present.index, count, std::addressof(priority)));
    }
    if (m_Graphics.index != m_Transfer.index)
    {
        ++count;
        createInfos.emplace_back(device_queue_create_info(m_Transfer.index, count, std::addressof(priority)));
    }
    if (m_Graphics.index != m_Compute.index)
    {
        ++count;
        createInfos.emplace_back(device_queue_create_info(m_Compute.index, count, std::addressof(priority)));
    }

    return createInfos;
}
void QueueFamilies::store_queue_handles(const Device& device)
{
    m_Present.handle = device.get_queue_handle(m_Present.index);
    m_Graphics.handle = device.get_queue_handle(m_Graphics.index);
    m_Compute.handle = device.get_queue_handle(m_Compute.index);
    m_Transfer.handle = device.get_queue_handle(m_Transfer.index);
}
void QueueFamilies::select_queue_indices(const PhysicalDevice& device, const Surface& surface)
{
    m_Present.index = select_present(device, surface);
    m_Graphics.index = select_graphics(device, surface);
    m_Compute.index = select_compute(device, surface);
    m_Transfer.index = select_transfer(device, surface);
}
}    // namespace odin::graphics::vk
