//
// Created by qwerty on 26/07/2025.
//
#include "PipelineRegistry.hpp"

#include "../../vk/pipeline/DescriptorSetLayoutBuilder.hpp"
#include "../../vk/pipeline/PipelineBuilder.hpp"
#include "../../vk/resource/VertexBuffer.hpp"
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
[[nodiscard]] std::uint64_t get_shader_hash(const std::optional<resource::ShaderHandle>& shader)
{
    return shader.has_value() ? shader->acquire()->hash() : 0;
}
}    // namespace
namespace odin::graphics::registry::pipeline
{
PipelineRegistry::PipelineRegistry(vk::DeviceRef device)
    : m_Device{ device }
    , m_DescriptorSetLayouts{}
    , m_PipelineLayouts{}
    , m_GraphicsPipelines{}
    , m_pMutex{ std::make_unique<mutex_t>() }
{}
GraphicsHandle PipelineRegistry::graphics_pipeline(const Request& request)
{
    auto [key, layoutKey] = make_pipeline_key(request);
    if (!m_GraphicsPipelines.contains(key))
    {
        add_graphics_pipeline(key, layoutKey, request);
    }

    auto pSlot = graphics_pipeline_slot(key);

    std::shared_ptr<const GraphicsResource> pShader = std::atomic_load(std::addressof(pSlot->pResource));
    if (pShader == nullptr)
    {
        std::lock_guard lock{ pSlot->initMutex };

        pShader = std::atomic_load(std::addressof(pSlot->pResource));
        if (pShader == nullptr)
        {
            pShader = std::make_shared<const GraphicsResource>();
            std::atomic_store(std::addressof(pSlot->pResource), std::move(pShader));
        }
    }

    return GraphicsHandle{ std::move(pSlot) };
}
void PipelineRegistry::add_descriptor_layout(const DescriptorSetLayoutKey& key)
{
    std::lock_guard lock{ *m_pMutex };

    // In case multiple threads are waiting on the above lock
    if (!m_DescriptorSetLayouts.contains(key))
    {
        vk::pipeline::DescriptorSetLayoutBuilder builder{};
        for (auto&& binding : key.bindings)
        {
            if (binding.bindingFlags & VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT)
            {
                bool partiallyBound = binding.bindingFlags & VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;
                bool updateAfterBind = binding.bindingFlags & VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;

                builder.add_runtime_descriptor_array(
                    binding.binding,
                    binding.descriptorType,
                    binding.descriptorCount,
                    binding.stageFlags,
                    partiallyBound,
                    updateAfterBind);
            }
            else
            {
                builder.add_binding(
                    binding.binding,
                    binding.descriptorType,
                    binding.descriptorCount,
                    binding.stageFlags,
                    binding.bindingFlags);
            }
        }

        auto [it, emplaced] = m_DescriptorSetLayouts.try_emplace(key, builder.build(m_Device));
    }
}
void PipelineRegistry::add_pipeline_layout(const PipelineLayoutKey& pipelineLayoutKey, std::span<const DescriptorSetLayoutKey> descKeys)
{
    std::lock_guard lock{ *m_pMutex };

    // In case multiple threads are waiting on the above lock
    if (!m_PipelineLayouts.contains(pipelineLayoutKey))
    {
        std::vector<vk::pipeline::DescriptorSetLayoutRef> layouts{};
        for (auto&& key : descKeys)
        {
            const vk::pipeline::DescriptorSetLayout& layout = m_DescriptorSetLayouts.at(key);
            layouts.emplace_back(layout.handle());
        }

        auto [it, emplaced] = m_PipelineLayouts.try_emplace(pipelineLayoutKey, m_Device, common::to_span(layouts));
    }
}
void PipelineRegistry::add_graphics_pipeline(const PipelineKey& key, const PipelineLayoutKey& layoutKey, const Request& request)
{
    std::lock_guard lock{ *m_pMutex };

    // In case multiple threads are waiting on the above lock
    if (!m_GraphicsPipelines.contains(key))
    {
        vk::pipeline::PipelineBuilder builder{ m_Device };

        if (request.stages.vs.has_value())
        {
            vk::resource::VertexDescription desc = vk::resource::VertexBuffer::get_vertex_description();
            builder.vertex_input_state(common::to_span(desc.bindings), common::to_span(desc.attributes));
        
            const resource::ShaderHandle& shader = request.stages.vs.value();
            builder.shader_module(shader, VK_SHADER_STAGE_VERTEX_BIT);
        }
        if (request.stages.fs.has_value())
        {
            const resource::ShaderHandle& shader = request.stages.fs.value();
            builder.shader_module(shader, VK_SHADER_STAGE_FRAGMENT_BIT);
        }
        if (request.stages.cs.has_value())
        {
            const resource::ShaderHandle& shader = request.stages.cs.value();
            builder.shader_module(shader, VK_SHADER_STAGE_COMPUTE_BIT);
        }
        VkSampleCountFlagBits samples = request.MsaaSamples.value_or(VK_SAMPLE_COUNT_1_BIT);
        builder.multisampling_state(samples);

        // TODO: Request has Polygon mode as std::optional - but the builder expects it to exist. Which one is right?
        // Since the builder will also be able to build compute shaders.. Maybe it does not have to exist for compute.
        // Builder will only enforce Polygon mode if calling builder.build_graphics_pipeline()
        if (request.polygonMode.has_value())
        {
            // bool depthBias = request.depthBias;  // TODO: request needs a depthBias
            VkPolygonMode mode = request.polygonMode.value();
            builder.rasterization_state(mode);
        }

        // Can span be empty or is it UB?
        std::span<const VkFormat> colorFormats{};
        if (request.colorFormats)
        {
            colorFormats = common::to_span(request.colorFormats.value());
        }
        builder.dynamic_rendering(colorFormats, request.depthFormat, request.stencilFormat);

        const vk::pipeline::PipelineLayout& layout = m_PipelineLayouts.at(layoutKey);
        vk::pipeline::GraphicsPipeline pipeline = builder.build_graphics_pipeline(layout.handle());
    
        auto[it, emplaced] = m_GraphicsPipelines.try_emplace(key, std::move(pipeline));
    }
}
std::shared_ptr<GraphicsSlot> PipelineRegistry::graphics_pipeline_slot(PipelineKey key)
{
    std::unique_lock lock{ *m_pMutex };

    auto [kvPair, emplaced] = m_GraphicsPipelines.try_emplace(key, std::weak_ptr<GraphicsSlot>{});
    if (std::shared_ptr<GraphicsSlot> pSlot = kvPair->second.lock(); pSlot != nullptr)
    {
        return pSlot;
    }

    auto pSlot = std::make_shared<GraphicsSlot>();
    kvPair->second = pSlot->weak_from_this();

    return pSlot;
}
std::tuple<PipelineKey, PipelineLayoutKey> PipelineRegistry::make_pipeline_key(const Request& request)
{
    PipelineKey key{};
    key.vsHash = get_shader_hash(request.stages.vs);
    key.fsHash = get_shader_hash(request.stages.fs);
    key.csHash = get_shader_hash(request.stages.cs);

    key.colorFormats = request.colorFormats;
    key.depthFormat = request.depthFormat;
    key.stencilFormat = request.stencilFormat;
    key.polygon = request.polygonMode;
    key.samples = request.MsaaSamples;

    std::vector<DescriptorSetLayoutKey> descSetLayoutKeys = make_descriptor_set_layout_key(request);
    PipelineLayoutKey pipelineLayoutKey = make_pipeline_layout_key(common::to_span(descSetLayoutKeys));

    PipelineLayoutKeyHasher hasher{};
    key.pipelineLayoutHash = hasher(pipelineLayoutKey);

    return { std::move(key), std::move(pipelineLayoutKey) };
}
std::vector<DescriptorSetLayoutKey> PipelineRegistry::make_descriptor_set_layout_key(const Request& request)
{
    std::vector<DescriptorSetLayoutKey> keys{};
    for (auto&& set : request.DescLayoutRequirement)
    {
        keys.emplace_back();
        for (auto&& bindingRequest : set)
        {
            DescriptorSetLayoutKey& key = keys.back();
            key.bindings.emplace_back();

            BindingKey& bindingKey = key.bindings.back();
            bindingKey.binding = bindingRequest.bindingID;
            bindingKey.descriptorType = bindingRequest.type;
            bindingKey.descriptorCount = bindingRequest.numDescriptors;
            bindingKey.bindingFlags = bindingRequest.flags;
            bindingKey.stageFlags = bindingRequest.stages;

            // TODO: in the future get sampler hash somehow
            // Request should probably hold a (optional) SamplerHandle from the ResourceRegistry
            bindingKey.immutableSamplerHash = 0;
        }
    }

    return keys;
}
PipelineLayoutKey PipelineRegistry::make_pipeline_layout_key(std::span<const DescriptorSetLayoutKey> descLayoutKeys)
{
    PipelineLayoutKey pk{};
    
    DescriptorSetLayoutKeyHasher hasher{};
    for (auto&& key : descLayoutKeys)
    {
        if (!m_DescriptorSetLayouts.contains(key))
        {
            add_descriptor_layout(key);
        }
        pk.descSetLayoutHashes.emplace_back(hasher(key));
    }

    // pk.pushConstants - Push Constants not allowed in the forseeable future
    pk.flags = VK_NO_FLAGS;     // Dont know what these flags do

    if (!m_PipelineLayouts.contains(pk))
    {
        add_pipeline_layout(pk, descLayoutKeys);
    }

    return pk;
}
}    // namespace odin::graphics::registry::pipeline
