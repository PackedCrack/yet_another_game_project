//
// Created by qwerty on 17/08/2025.
//
#include "PipelineLayout.hpp"

#include "../vulkan_defines.hpp"
#include "../vulkan_info.hpp"
// common
#include <common.hpp>
//
//
namespace
{
using namespace odin::graphics::vk::pipeline;
//
//
[[nodiscard]] VkPipelineLayoutCreateInfo make_layout_create_info(std::span<const VkPushConstantRange> pushConstantRanges,
                                                                 std::span<const VkDescriptorSetLayout> descriptorLayouts)
{
    VkPipelineLayoutCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = VK_NO_FLAGS;
    info.setLayoutCount = static_cast<std::uint32_t>(descriptorLayouts.size());
    info.pSetLayouts = descriptorLayouts.empty() ? nullptr : descriptorLayouts.data();
    info.pushConstantRangeCount = static_cast<std::uint32_t>(pushConstantRanges.size());
    info.pPushConstantRanges = pushConstantRanges.empty() ? nullptr : pushConstantRanges.data();

    return info;
}
}    // namespace
namespace odin::graphics::vk::pipeline
{
PipelineLayout::PipelineLayout(DeviceRef device,
                               std::span<const DescriptorSetLayoutRef> descriptorLayouts,
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

    std::vector<VkDescriptorSetLayout> layouts{};
    std::transform(std::begin(descriptorLayouts),
                   std::end(descriptorLayouts),
                   std::back_inserter(layouts),
                   [](DescriptorSetLayoutRef layout) { return layout.handle; });

    VkPipelineLayoutCreateInfo info = make_layout_create_info(common::to_span(ranges), common::to_span(layouts));
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
