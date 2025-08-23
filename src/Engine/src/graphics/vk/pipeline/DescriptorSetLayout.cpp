//
// Created by qwerty on 17/08/2025.
//
#include "DescriptorSetLayout.hpp"

#include "../vulkan_defines.hpp"
#include "../vulkan_info.hpp"
//
//
namespace odin::graphics::vk::pipeline
{
DescriptorSetLayout::DescriptorSetLayout(DeviceRef device, const VkDescriptorSetLayoutCreateInfo& createInfo)
    : m_DescriptorSetLayout{ VK_NULL_HANDLE }
    , m_Device{ device }
{
    VK_CHECK(vkCreateDescriptorSetLayout(m_Device.handle, std::addressof(createInfo), nullptr, std::addressof(m_DescriptorSetLayout)),
             "Failed to create Descriptor Set Layout.");
}
DescriptorSetLayout::~DescriptorSetLayout()
{
    if (m_DescriptorSetLayout != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorSetLayout(m_Device.handle, m_DescriptorSetLayout, nullptr);
    }
}
DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout&& other) noexcept
    : m_DescriptorSetLayout{ VK_NULL_HANDLE }
    , m_Device{ other.m_Device }
{
    std::swap(m_DescriptorSetLayout, other.m_DescriptorSetLayout);
}
DescriptorSetLayout& DescriptorSetLayout::operator=(DescriptorSetLayout&& other) noexcept
{
    if (this != std::addressof(other))
    {
        std::swap(m_DescriptorSetLayout, other.m_DescriptorSetLayout);
        m_Device = other.m_Device;
    }

    return *this;
}
DescriptorSetLayoutRef DescriptorSetLayout::handle() const
{
    ODIN_ASSERT(m_DescriptorSetLayout != VK_NULL_HANDLE);
    return DescriptorSetLayoutRef{ .handle = m_DescriptorSetLayout };
}
}    // namespace odin::graphics::vk::pipeline
