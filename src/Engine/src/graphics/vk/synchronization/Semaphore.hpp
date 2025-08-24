//
// Created by qwerty on 15/08/2025.
//
#pragma once

#include "../Device.hpp"
#include "../vulkan_defines.hpp"
//
//
namespace odin::graphics::vk::synchronization
{
struct SemaphoreRef
{
    VkSemaphore handle;
};
class Semaphore
{
public:
    Semaphore(DeviceRef device);
    ~Semaphore();
    Semaphore(const Semaphore& other) = delete;
    Semaphore(Semaphore&& other) noexcept;
    Semaphore& operator=(const Semaphore& other) = delete;
    Semaphore& operator=(Semaphore&& other) noexcept;
public:
    [[nodiscard]] SemaphoreRef handle() const;
    template<typename... flag_args_t>
    requires(PipelineStageFlag2<flag_args_t> && ...)
    [[nodiscard]] VkSemaphoreSubmitInfo submit_info(flag_args_t... stages) const
    {
        static_assert(sizeof...(stages) > 0, "Atleast 1 stage is requried");
        ODIN_ASSERT(m_Semaphore != VK_NULL_HANDLE);
        return VkSemaphoreSubmitInfo{ .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
                                      .pNext = nullptr,
                                      .semaphore = m_Semaphore,
                                      .value = 0,
                                      .stageMask = (stages | ...),
                                      .deviceIndex = 0 };
    }
private:
    VkSemaphore m_Semaphore = VK_NULL_HANDLE;
    DeviceRef m_Device;
};
}    // namespace odin::graphics::vk::synchronization
