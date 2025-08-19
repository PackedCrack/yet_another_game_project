//
// Created by qwerty on 15/08/2025.
//
#include "TimelineSemaphore.hpp"

#include "../vulkan_defines.hpp"
#include "../vulkan_info.hpp"
//
//
namespace odin::graphics::vk::synchronization
{
TimelineSemaphore::TimelineSemaphore(DeviceRef device)
    : m_Semaphore{ VK_NULL_HANDLE }
    , m_Device{ device }
{
    static constexpr VkSemaphoreTypeCreateInfo timelineInfo{ .sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
                                                             .pNext = nullptr,
                                                             .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
                                                             .initialValue = 0 };
    static constexpr VkSemaphoreCreateInfo info = { .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
                                                    .pNext = std::addressof(timelineInfo),
                                                    .flags = VK_NO_FLAGS };
    VK_CHECK(vkCreateSemaphore(m_Device.handle, std::addressof(info), nullptr, std::addressof(m_Semaphore)),
             "Failed to create Vulkan Timeline Semaphore");
}
TimelineSemaphore::~TimelineSemaphore()
{
    if (m_Semaphore != VK_NULL_HANDLE)
    {
        vkDestroySemaphore(m_Device.handle, m_Semaphore, nullptr);
    }
}
TimelineSemaphore::TimelineSemaphore(TimelineSemaphore&& other) noexcept
    : m_Semaphore{ VK_NULL_HANDLE }
    , m_Device{ other.m_Device }
{
    std::swap(m_Semaphore, other.m_Semaphore);
}
TimelineSemaphore& TimelineSemaphore::operator=(TimelineSemaphore&& other) noexcept
{
    if (this != std::addressof(other))
    {
        m_Semaphore = std::exchange(other.m_Semaphore, m_Semaphore);
        m_Device = std::exchange(other.m_Device, m_Device);
    }

    return *this;
}
TimelineSemaphoreRef TimelineSemaphore::handle() const
{
    ODIN_ASSERT(m_Semaphore != VK_NULL_HANDLE);
    return TimelineSemaphoreRef{ .handle = m_Semaphore };
}
}    // namespace odin::graphics::vk::synchronization
