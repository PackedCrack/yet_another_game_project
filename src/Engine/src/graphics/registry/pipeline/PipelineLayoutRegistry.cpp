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
[[nodiscard]] std::vector<std::uint64_t> hash_descriptor_layout_keys(std::span<descriptors::DescriptorSetLayoutKey> keys)
{
    descriptors::DescriptorSetLayoutKeyHasher hasher{};
    std::vector<std::uint64_t> hashes{};
    std::transform(std::begin(keys),
                   std::end(keys),
                   std::back_inserter(hashes),
                   [&hasher](const descriptors::DescriptorSetLayoutKey& key) { return hasher(key); });

    return hashes;
}
}    // namespace
namespace odin::graphics::registry::pipeline
{
PipelineLayoutRegistry::PipelineLayoutRegistry(vk::DeviceRef device)
    : m_Device{ device }
    , m_DescriptorLayouts{ m_Device }
    , m_Layouts{}
    , m_pMutex{ std::make_unique<std::mutex>() }
{}
PipelineLayoutKey PipelineLayoutRegistry::make_layout_key(const Request& request)
{
    PipelineLayoutKey pk{};
    pk.descriptorLayoutKeys = m_DescriptorLayouts.make_layout_keys(request);
    // pk.pushConstants - Push Constants not allowed in the forseeable future
    pk.flags = VK_NO_FLAGS;    // Dont know what these flags do

    if (!m_Layouts.contains(pk))
    {
        add_pipeline_layout(pk, common::to_span(pk.descriptorLayoutKeys));
    }

    return pk;
}
vk::pipeline::PipelineLayoutRef PipelineLayoutRegistry::pipeline_layout(const PipelineLayoutKey& key)
{
    return m_Layouts.at(key).handle();
}
std::vector<vk::pipeline::DescriptorSetLayoutRef> PipelineLayoutRegistry::descriptor_set_layouts(const PipelineLayoutKey& key)
{
    const auto& setLayoutKeys = key.descriptorLayoutKeys;
    return m_DescriptorLayouts.descriptor_set_layouts(setLayoutKeys);
}
bool PipelineLayoutRegistry::update_after_bind(const PipelineLayoutKey& key, std::uint32_t setID) const
{
    ODIN_ASSERT(setID < key.descriptorLayoutKeys.size());

    const descriptors::DescriptorSetLayoutKey& setKey = key.descriptorLayoutKeys[setID];
    return m_DescriptorLayouts.requires_update_after_bind(setKey);
}
void PipelineLayoutRegistry::add_pipeline_layout(const PipelineLayoutKey& pipelineLayoutKey,
                                                 std::span<const descriptors::DescriptorSetLayoutKey> descKeys)
{
    std::lock_guard lock{ *m_pMutex };

    // In case multiple threads are waiting on the above lock
    if (!m_Layouts.contains(pipelineLayoutKey))
    {
        auto layouts = m_DescriptorLayouts.descriptor_set_layouts(descKeys);
        auto [it, emplaced] = m_Layouts.try_emplace(pipelineLayoutKey, m_Device, common::to_span(layouts));
    }
}
}    // namespace odin::graphics::registry::pipeline
