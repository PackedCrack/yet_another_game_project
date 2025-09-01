//
// Created by qwerty on 25/08/2025.
//
#include "DescriptorSetAllocator.hpp"

#include "../vulkan_defines.hpp"
//
//
namespace
{
using namespace odin::graphics::vk::pipeline;
constexpr bool updateAfterBind = true;
//
//
[[nodiscard]] VkDescriptorSetAllocateInfo make_alloc_info(DescriptorPoolRef pool, const DescriptorSetLayoutRef& layout, const VkDescriptorSetVariableDescriptorCountAllocateInfo& variableCountInfo)
{
    VkDescriptorSetAllocateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    info.pNext = std::addressof(variableCountInfo);
    info.descriptorPool = pool.handle;
    info.descriptorSetCount = 1;
    info.pSetLayouts = std::addressof(layout.handle);

    return info;
}
}    // namespace
namespace odin::graphics::vk::pipeline
{
DescriptorSetAllocator::DescriptorSetAllocator(DeviceRef device)
    : m_Pool{ device }
    , m_PoolWithUAB{ device, updateAfterBind }
    , m_Device{ device }
{}
VkDescriptorSet DescriptorSetAllocator::alloc(DescriptorSetLayoutRef layout, bool requiresUpdateAfterBind, std::uint32_t variableCount)
{
    // TODO: This is for bindless Textures - which is a future project
    ODIN_ASSERT(variableCount == 0);
    VkDescriptorSetVariableDescriptorCountAllocateInfo varInfo{};
    varInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO;
    varInfo.pNext = nullptr;
    varInfo.descriptorSetCount = variableCount;
    varInfo.pDescriptorCounts = nullptr;

    DescriptorPoolRef pool = requiresUpdateAfterBind ? m_PoolWithUAB.handle() : m_Pool.handle();
    VkDescriptorSetAllocateInfo info = make_alloc_info(pool, layout, varInfo);

    VkDescriptorSet set{};
    VK_CHECK(vkAllocateDescriptorSets(m_Device.handle, std::addressof(info), std::addressof(set)), "Failed to allocate Descriptor Set.");

    return set;
}
}    // namespace odin::graphics::vk::pipeline
