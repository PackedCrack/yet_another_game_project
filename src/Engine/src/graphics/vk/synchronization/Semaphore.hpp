//
// Created by qwerty on 15/08/2025.
//
#pragma once

#include "../Device.hpp"
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
private:
    VkSemaphore m_Semaphore = VK_NULL_HANDLE;
    DeviceRef m_Device;
};
}    // namespace odin::graphics::vk::synchronization
