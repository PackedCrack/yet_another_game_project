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
    // pk.pushConstants - Push Constants not allowed in the forseeable future
    pk.flags = VK_NO_FLAGS;    // Dont know what these flags do

    if (!m_Layouts.contains(pk))
    {
        add_pipeline_layout(pk, descriptorLayoutRegistry, common::to_span(pk.descriptorLayoutKeys));
    }

    return pk;
}
vk::pipeline::PipelineLayoutRef PipelineLayoutRegistry::pipeline_layout(const PipelineLayoutKey& key) const
{
    return m_Layouts.at(key).handle();
}
void PipelineLayoutRegistry::add_pipeline_layout(const PipelineLayoutKey& pipelineLayoutKey,
                                                 DescriptorSetLayoutRegistry& descriptorLayoutRegistry,
                                                 std::span<const DescriptorSetLayoutKey> descKeys)
{
    std::lock_guard lock{ *m_pMutex };

    // In case multiple threads are waiting on the above lock
    if (!m_Layouts.contains(pipelineLayoutKey))
    {
        auto layouts = descriptorLayoutRegistry.descriptor_set_layouts(descKeys);
        auto [it, emplaced] = m_Layouts.try_emplace(pipelineLayoutKey, m_Device, common::to_span(layouts));
    }
}
}    // namespace odin::graphics::registry::pipeline
