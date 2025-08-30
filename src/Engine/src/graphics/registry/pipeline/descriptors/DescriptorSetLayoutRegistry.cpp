//
// Created by qwerty on 28/08/2025.
//
#include "DescriptorSetLayoutRegistry.hpp"

#include "../../../vk/pipeline/DescriptorSetLayoutBuilder.hpp"
//
//
namespace
{
using namespace odin::graphics::registry::pipeline;
using namespace odin::graphics::registry::pipeline::descriptors;
using DescriptorSetLayoutBuilder = odin::graphics::vk::pipeline::DescriptorSetLayoutBuilder;
//
//
[[nodiscard]] bool has_variable_descriptor_count(const Binding& binding)
{
    return binding.bindingFlags & VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT;
}
[[nodiscard]] bool requires_partially_bound(const Binding& binding)
{
    return binding.bindingFlags & VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;
}
[[nodiscard]] bool requires_update_after_bind(const Binding& binding)
{
    return binding.bindingFlags & VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
}
[[nodiscard]] DescriptorSetLayoutBuilder& add_runtime_array(DescriptorSetLayoutBuilder& builder, const Binding& binding)
{
    bool partiallyBound = requires_partially_bound(binding);
    bool updateAfterBind = requires_update_after_bind(binding);

    builder.add_runtime_descriptor_array(binding.binding,
                                         binding.descriptorType,
                                         binding.descriptorCount,
                                         binding.stageFlags,
                                         partiallyBound,
                                         updateAfterBind);

    return builder;
}
[[nodiscard]] DescriptorSetLayoutBuilder& add_binding(DescriptorSetLayoutBuilder& builder, const Binding& binding)
{
    builder.add_binding(binding.binding, binding.descriptorType, binding.descriptorCount, binding.stageFlags, binding.bindingFlags);

    return builder;
}
[[nodiscard]] Binding make_binding(const DescriptorRequest& request)
{
    Binding binding{};
    binding.binding = request.bindingID;
    binding.descriptorType = request.type;
    binding.descriptorCount = request.numDescriptors;
    binding.bindingFlags = request.flags;
    binding.stageFlags = request.stages;

    // TODO: in the future get sampler hash somehow
    // Request should probably hold a (optional) SamplerHandle from the ResourceRegistry
    binding.immutableSamplerHash = 0;

    return binding;
}
}    // namespace
namespace odin::graphics::registry::pipeline::descriptors
{
DescriptorSetLayoutRegistry::DescriptorSetLayoutRegistry(vk::DeviceRef device)
    : m_Device{ device }
    , m_Layouts{}
    , m_pMutex{ std::make_unique<std::mutex>() }
{}
std::vector<vk::pipeline::DescriptorSetLayoutRef>
DescriptorSetLayoutRegistry::descriptor_set_layouts(std::span<const DescriptorSetLayoutKey> keys)
{
    std::vector<vk::pipeline::DescriptorSetLayoutRef> refs{};
    std::transform(std::begin(keys),
                   std::end(keys),
                   std::back_inserter(refs),
                   [this](const DescriptorSetLayoutKey& key)
                   {
                       ODIN_ASSERT(m_Layouts.contains(key));
                       return m_Layouts.at(key).handle();
                   });

    return refs;
}
std::vector<DescriptorSetLayoutKey> DescriptorSetLayoutRegistry::make_layout_keys(const Request& request)
{
    std::vector<DescriptorSetLayoutKey> keys{};
    for (auto&& set : request.DescLayoutRequirement)
    {
        keys.emplace_back();
        DescriptorSetLayoutKey& key = keys.back();
        for (auto&& bindingRequest : set)
        {
            key.bindings.emplace_back(make_binding(bindingRequest));
        }

        if (!m_Layouts.contains(key))
        {
            add_descriptor_layout(key);
        }
    }

    return keys;
}
bool DescriptorSetLayoutRegistry::requires_update_after_bind(const DescriptorSetLayoutKey& key) const
{
    return m_Layouts.at(key).requires_update_after_bind();
}
void DescriptorSetLayoutRegistry::add_descriptor_layout(const DescriptorSetLayoutKey& key)
{
    std::lock_guard lock{ *m_pMutex };

    // In case multiple threads are waiting on the above lock
    if (!m_Layouts.contains(key))
    {
        vk::pipeline::DescriptorSetLayoutBuilder builder{};
        for (auto&& binding : key.bindings)
        {
            if (has_variable_descriptor_count(binding))
            {
                builder = add_runtime_array(builder, binding);
            }
            else
            {
                builder = add_binding(builder, binding);
            }
        }

        auto [it, emplaced] = m_Layouts.try_emplace(key, builder.build(m_Device));
        ODIN_ASSERT(emplaced);
    }
}
}    // namespace odin::graphics::registry::pipeline::descriptors
