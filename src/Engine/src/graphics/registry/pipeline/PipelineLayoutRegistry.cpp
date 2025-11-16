//
// Created by qwerty on 27/08/2025.
//
#include "PipelineLayoutRegistry.hpp"

// common
#include <common.hpp>
//
//
namespace
{
using namespace odin::graphics::registry;
using namespace odin::graphics::registry::pipeline;
//
//
[[nodiscard]] std::vector<std::uint64_t> hash_descriptor_layout_keys(std::span<DescriptorSetLayoutKey> keys)
{
    DescriptorSetLayoutKeyHasher hasher{};
    std::vector<std::uint64_t> hashes{};
    std::transform(std::begin(keys),
                   std::end(keys),
                   std::back_inserter(hashes),
                   [&hasher](const DescriptorSetLayoutKey& key) { return hasher(key); });

    return hashes;
}
[[nodiscard]] std::vector<VkPushConstantRange> make_push_constant_ranges(const Request& request)
{
    if (!request.pushConstants.has_value())
    {
        return {};
    }

    // Just copy.. how often would this really be called
    return request.pushConstants.value();
}
}    // namespace
namespace odin::graphics::registry::pipeline
{
PipelineLayoutRegistry::PipelineLayoutRegistry(vk::DeviceRef device)
    : m_Device{ device }
    , m_Layouts{}
    , m_pMutex{ std::make_unique<std::mutex>() }
{}
PipelineLayoutKey PipelineLayoutRegistry::make_layout_key(const Request& request, DescriptorSetLayoutRegistry& descriptorLayoutRegistry)
{
    PipelineLayoutKey pk{};
    pk.descriptorLayoutKeys = descriptorLayoutRegistry.make_layout_keys(request);
    pk.pushConstants = make_push_constant_ranges(request);
    pk.flags = VK_NO_FLAGS;    // Dont know what these flags do

    if (!m_Layouts.contains(pk))
    {
        add_pipeline_layout(pk, descriptorLayoutRegistry);
    }

    return pk;
}
vk::pipeline::PipelineLayoutRef PipelineLayoutRegistry::pipeline_layout(const PipelineLayoutKey& key) const
{
    return m_Layouts.at(key).handle();
}
void PipelineLayoutRegistry::add_pipeline_layout(const PipelineLayoutKey& pipelineLayoutKey,
                                                 DescriptorSetLayoutRegistry& descriptorLayoutRegistry)
{
    std::lock_guard lock{ *m_pMutex };

    // In case multiple threads are waiting on the above lock
    if (!m_Layouts.contains(pipelineLayoutKey))
    {
        std::span<const DescriptorSetLayoutKey> descKeys = pipelineLayoutKey.descriptorLayoutKeys;
        std::span<const VkPushConstantRange> pushConstants = pipelineLayoutKey.pushConstants;
        auto layouts = descriptorLayoutRegistry.descriptor_set_layouts(descKeys);
        auto [it, emplaced] = m_Layouts.try_emplace(pipelineLayoutKey, m_Device, layouts, pushConstants);
    }
}
}    // namespace odin::graphics::registry::pipeline
