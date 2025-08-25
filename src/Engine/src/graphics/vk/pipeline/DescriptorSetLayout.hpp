//
// Created by qwerty on 17/08/2025.
//
#pragma once

#include "../Device.hpp"
//
//
namespace odin::graphics::vk::pipeline
{
struct DescriptorSetLayoutRef
{
    VkDescriptorSetLayout handle;
};
class DescriptorSetLayout
{
public:
    DescriptorSetLayout(DeviceRef device, const VkDescriptorSetLayoutCreateInfo& info, bool updateAfterBind);
    ~DescriptorSetLayout();
    DescriptorSetLayout(const DescriptorSetLayout& other) = delete;
    DescriptorSetLayout(DescriptorSetLayout&& other) noexcept;
    DescriptorSetLayout& operator=(const DescriptorSetLayout& other) = delete;
    DescriptorSetLayout& operator=(DescriptorSetLayout&& other) noexcept;
public:
    [[nodiscard]] DescriptorSetLayoutRef handle() const;
    [[nodiscard]] bool requires_update_after_bind() const;
private:
    VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;
    DeviceRef m_Device;
    bool m_UpdateAfterBind;
};
}    // namespace odin::graphics::vk::pipeline
