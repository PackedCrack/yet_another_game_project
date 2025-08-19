//
// Created by qwerty on 17/08/2025.
//
#pragma once

#include "../Device.hpp"
//
//
namespace odin::graphics::vk::pipeline
{
struct DescriptorPoolRef
{
    VkDescriptorPool handle;
};
class DescriptorPool
{
public:
    DescriptorPool(DeviceRef device);
    ~DescriptorPool();
    DescriptorPool(const DescriptorPool& other) = delete;
    DescriptorPool(DescriptorPool&& other) noexcept;
    DescriptorPool& operator=(const DescriptorPool& other) = delete;
    DescriptorPool& operator=(DescriptorPool&& other) noexcept;
public:
    [[nodiscard]] DescriptorPoolRef handle() const;
private:
    VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;
    DeviceRef m_Device;
};
}    // namespace odin::graphics::vk::pipeline
