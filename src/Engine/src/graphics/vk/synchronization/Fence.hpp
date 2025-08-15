#pragma once

#include "../Device.hpp"
//
//
namespace odin::graphics::vk::synchronization
{
struct FenceRef
{
    VkFence handle;
};
class Fence
{
public:
    Fence(DeviceRef device, bool startSignaled = true);
    ~Fence();
    Fence(const Fence& other) = delete;
    Fence(Fence&& other) noexcept;
    Fence& operator=(const Fence& other) = delete;
    Fence& operator=(Fence&& other) noexcept;

    [[nodiscard]] FenceRef handle() const;
private:
    VkFence m_Fence = VK_NULL_HANDLE;
    DeviceRef m_Device;
};
}    // namespace odin::graphics::vk::synchronization
