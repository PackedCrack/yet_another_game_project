//
// Created by qwerty on 17/08/2025.
//
#include "PipelineLayout.hpp"

#include "../vulkan_defines.hpp"
#include "../vulkan_info.hpp"
//
//
namespace odin::graphics::vk::pipeline
{
PipelineLayout::PipelineLayout(DeviceRef device,
                               const std::vector<VkDescriptorSetLayout>& descriptorLayouts,
                               std::optional<std::reference_wrapper<std::vector<VkPushConstantRange>>> pushContantRanges)
    : m_Layout{ VK_NULL_HANDLE }
    , m_Device{ device }
{
    std::vector<VkPushConstantRange> ranges{};
    if (pushContantRanges)
    {
        // Just copy.. how often would this really be called?
        ranges = pushContantRanges->get();
    }

    VkPipelineLayoutCreateInfo info = pipeline_layout_create_info(ranges, descriptorLayouts);
    VK_CHECK(vkCreatePipelineLayout(m_Device.handle, std::addressof(info), nullptr, std::addressof(m_Layout)),
             "Failed to create Vulkan Pipeline Layout");
}
PipelineLayout::~PipelineLayout()
{
    if (m_Layout != VK_NULL_HANDLE)
    {
        vkDestroyPipelineLayout(m_Device.handle, m_Layout, nullptr);
    }
}
PipelineLayout::PipelineLayout(PipelineLayout&& other) noexcept
    : m_Layout{ VK_NULL_HANDLE }
    , m_Device{ other.m_Device }
{
    std::swap(m_Layout, other.m_Layout);
}
PipelineLayout& PipelineLayout::operator=(PipelineLayout&& other) noexcept
{
    if (this != std::addressof(other))
    {
        std::swap(m_Layout, other.m_Layout);
        m_Device = other.m_Device;
    }

    return *this;
}
PipelineLayoutRef PipelineLayout::handle() const
{
    ODIN_ASSERT(m_Layout != VK_NULL_HANDLE);
    return PipelineLayoutRef{ .handle = m_Layout };
}
}    // namespace odin::graphics::vk::pipeline
