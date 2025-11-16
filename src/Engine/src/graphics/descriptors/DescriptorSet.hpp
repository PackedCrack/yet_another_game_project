//
// Created by qwerty on 07/09/2025.
//
#pragma once

#include "../vk/CommandBuffer.hpp"
#include "../vk/pipeline/PipelineLayout.hpp"
#include "../registry/pipeline/PipelineRegistry.hpp"
//
//
namespace odin::graphics::descriptors
{
template<typename derived_t>
class DescriptorSet
{
public:
    DescriptorSet(const registry::pipeline::Request& request, registry::pipeline::PipelineRegistry& pipelines)
        : m_Set{ VK_NULL_HANDLE }
    {
        std::vector<registry::pipeline::DescriptorSetLayoutKey> layouts = pipelines.make_descriptor_layouts(request);
        ODIN_ASSERT(layouts.size() == 1);
        m_Set = pipelines.allocate_descriptor_set(layouts.front());
    }
protected:
    void bind(vk::CommandBufferRef cmdBuffer,
              const vk::pipeline::PipelineLayoutRef layout,
              VkShaderStageFlags stages,
              std::uint32_t setID,
              std::uint32_t* pOffsets,
              std::uint32_t offsetCount) const
    {
        VkBindDescriptorSetsInfo info{};
        info.sType = VK_STRUCTURE_TYPE_BIND_DESCRIPTOR_SETS_INFO;
        info.pNext = nullptr;
        info.stageFlags = stages;
        info.layout = layout.handle;
        info.firstSet = setID;
        info.descriptorSetCount = 1;
        info.pDescriptorSets = std::addressof(m_Set);
        info.dynamicOffsetCount = offsetCount;
        info.pDynamicOffsets = pOffsets;

        vkCmdBindDescriptorSets2(cmdBuffer.handle, std::addressof(info));
    }
protected:
    VkDescriptorSet m_Set;
};
}    // namespace odin::graphics::descriptors
