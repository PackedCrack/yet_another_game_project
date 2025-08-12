#include "QueueFamilies.hpp"

#include "vulkan_defines.hpp"
//
//
namespace
{
using index_t = odin::graphics::vk::QueueFamilies::index_t;
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
index_t select_present(const odin::graphics::vk::PhysicalDevice& device, const odin::graphics::vk::Surface& surface)
{
    const std::vector<VkQueueFamilyProperties>& properties = device.queue_families_properties();

    std::optional<index_t> bestCandidate{};
    std::optional<index_t> backup{};
    for (std::size_t i = 0; i < properties.size(); ++i)
    {
        const VkQueueFamilyProperties& property = properties[i];
        if (surface.queue_family_supports_present(device.handle(), i))
        {
            if (!backup.has_value())
            {
                backup = std::make_optional(i);
            }

            if (family_supports_graphics(property) && family_supports_compute(property))
            {
                bestCandidate = std::make_optional(i);
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
index_t select_graphics(const odin::graphics::vk::PhysicalDevice& device, const odin::graphics::vk::Surface& surface)
{
    const std::vector<VkQueueFamilyProperties>& properties = device.queue_families_properties();

    std::optional<index_t> bestCandidate{};
    std::optional<index_t> backup{};
    for (std::size_t i = 0; i < properties.size(); ++i)
    {
        const VkQueueFamilyProperties& property = properties[i];
        if (family_supports_graphics(property))
        {
            if (!backup.has_value())
            {
                backup = std::make_optional(i);
            }

            if (family_supports_compute(property) && surface.queue_family_supports_present(device.handle(), i))
            {
                bestCandidate = std::make_optional(i);
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
index_t select_compute(const odin::graphics::vk::PhysicalDevice& device, const odin::graphics::vk::Surface& surface)
{
    const std::vector<VkQueueFamilyProperties>& properties = device.queue_families_properties();

    std::optional<index_t> bestCandidate{};
    std::optional<index_t> backup{};
    for (std::size_t i = 0; i < properties.size(); ++i)
    {
        const VkQueueFamilyProperties& property = properties[i];
        if (family_supports_compute(property))
        {
            if (!backup.has_value())
            {
                backup = std::make_optional(i);
            }

            if (family_supports_graphics(property) && surface.queue_family_supports_present(device.handle(), i))
            {
                bestCandidate = std::make_optional(i);
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
index_t select_transfer(const odin::graphics::vk::PhysicalDevice& device, const odin::graphics::vk::Surface& surface)
{
    const std::vector<VkQueueFamilyProperties>& properties = device.queue_families_properties();

    std::optional<index_t> bestCandidate{};
    std::optional<index_t> backup{};
    for (std::size_t i = 0; i < properties.size(); ++i)
    {
        const VkQueueFamilyProperties& property = properties[i];
        if (family_supports_transfer(property))
        {
            if (!backup.has_value())
            {
                backup = std::make_optional(i);
            }

            if (!family_supports_graphics(property) && 
                !surface.queue_family_supports_present(device.handle(), i) &&
                !family_supports_compute(property))
            {
                bestCandidate = std::make_optional(i);
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
}	// namespace
namespace odin::graphics::vk
{
QueueFamilies::QueueFamilies(const PhysicalDevice& device, const Surface& surface)
	: m_Present{ VK_NULL_HANDLE, INVALID_INDEX }
	, m_Graphics{ VK_NULL_HANDLE, INVALID_INDEX }
	, m_Compute{ VK_NULL_HANDLE, INVALID_INDEX }
	, m_Transfer{ VK_NULL_HANDLE, INVALID_INDEX }
{
    select_queue_indices(device, surface);
}
VkQueue QueueFamilies::present() const
{
	ODIN_ASSERT(m_Present.handle != VK_NULL_HANDLE);
	return m_Present.handle;
}
VkQueue QueueFamilies::graphics() const
{
	ODIN_ASSERT(m_Graphics.handle != VK_NULL_HANDLE);
	return m_Graphics.handle;
}
VkQueue QueueFamilies::compute() const
{
	ODIN_ASSERT(m_Compute.handle != VK_NULL_HANDLE);
	return m_Compute.handle;
}
VkQueue QueueFamilies::transfer() const
{
	ODIN_ASSERT(m_Transfer.handle != VK_NULL_HANDLE);
	return m_Transfer.handle;
}
void QueueFamilies::select_queue_indices(const PhysicalDevice& device, const Surface& surface)
{
    m_Present.index = select_present(device, surface);
    m_Graphics.index = select_graphics(device, surface);
    m_Compute.index = select_compute(device, surface);
    m_Transfer.index = select_transfer(device, surface);
}
}	// namespace odin::graphics::vk