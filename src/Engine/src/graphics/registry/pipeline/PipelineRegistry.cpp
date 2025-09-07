//
// Created by qwerty on 26/08/2025.
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
using namespace odin::graphics;
using namespace odin::graphics::registry;
using namespace odin::graphics::registry::pipeline;
using GraphicsPipeline = vk::pipeline::GraphicsPipeline;
using PipelineLayoutRef = vk::pipeline::PipelineLayoutRef;
//
//
[[nodiscard]] std::uint64_t get_shader_hash(const std::optional<resource::shader::ShaderHandle>& shader)
{
    return shader.has_value() ? shader->acquire()->hash() : 0;
}
[[nodiscard]] GraphicsPipeline build_graphics_pipeline(vk::DeviceRef device, const Request& request, PipelineLayoutRef layout)
{
    using PipelineBuilder = vk::pipeline::PipelineBuilder;

    PipelineBuilder builder{ device };
    vk::resource::VertexDescription desc = vk::resource::VertexBuffer::get_vertex_description();
    if (request.vs.has_value())
    {
        builder.vertex_input_state(common::to_span(desc.bindings), common::to_span(desc.attributes));

        const resource::shader::ShaderHandle& shader = request.vs.value();
        builder.shader_module(shader, VK_SHADER_STAGE_VERTEX_BIT);
    }
    if (request.fs.has_value())
    {
        const resource::shader::ShaderHandle& shader = request.fs.value();
        builder.shader_module(shader, VK_SHADER_STAGE_FRAGMENT_BIT);
    }
    ODIN_ASSERT(!request.cs.has_value());

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
        colorFormats = request.colorFormats.value();
    }
    builder.dynamic_rendering(colorFormats, request.depthFormat, request.stencilFormat);

    return builder.build_graphics_pipeline(layout);
}
[[nodiscard]] bool vertex_shader_missmatch(const std::shared_ptr<GraphicsSlot>& pSlot,
                                           const std::shared_ptr<const GraphicsResource>& pResource)
{
    std::shared_ptr<const vk::resource::ShaderModule> pVsShader = pSlot->request.vs->acquire();
    return pResource->vsHash.value() != pVsShader->hash();
}
[[nodiscard]] bool fragment_shader_missmatch(const std::shared_ptr<GraphicsSlot>& pSlot,
                                             const std::shared_ptr<const GraphicsResource>& pResource)
{
    std::shared_ptr<const vk::resource::ShaderModule> pFsShader = pSlot->request.fs->acquire();
    return pResource->fsHash.value() != pFsShader->hash();
}
[[nodiscard]] bool compute_shader_missmatch(const std::shared_ptr<GraphicsSlot>& pSlot,
                                            const std::shared_ptr<const GraphicsResource>& pResource)
{
    std::shared_ptr<const vk::resource::ShaderModule> pCsShader = pSlot->request.cs->acquire();
    return pResource->csHash.value() != pCsShader->hash();
}
[[nodiscard]] bool shader_outdated(const std::shared_ptr<GraphicsSlot>& pSlot)
{
    if (pSlot == nullptr)
    {
        return true;
    }

    auto pResource = std::atomic_load(std::addressof(pSlot->pResource));
    if (pResource == nullptr)
    {
        return true;
    }
    if (pResource->vsHash)
    {
        if (vertex_shader_missmatch(pSlot, pResource))
        {
            return true;
        }
    }
    if (pResource->fsHash)
    {
        if (fragment_shader_missmatch(pSlot, pResource))
        {
            return true;
        }
    }
    if (pResource->csHash)
    {
        if (compute_shader_missmatch(pSlot, pResource))
        {
            return true;
        }
    }

    return false;
}
}    // namespace
namespace odin::graphics::registry::pipeline
{
std::unique_ptr<PipelineRegistry> PipelineRegistry::make(vk::DeviceRef device)
{
    return std::make_unique<PipelineRegistry>(PipelineRegistry{ device });
}
PipelineRegistry::PipelineRegistry(vk::DeviceRef device)
    : m_Device{ device }
    , m_PipelineLayouts{ m_Device }
    , m_DescriptorLayouts{ m_Device }
    , m_DescriptorAllocator{ m_Device }
    , m_GraphicsPipelines{}
    , m_pMutex{ std::make_unique<mutex_t>() }
{}
//VkDescriptorSet PipelineRegistry::allocate_descriptor_set(GraphicsHandle handle, std::uint32_t setID)
//{
//    using DescriptorSetLayoutRef = vk::pipeline::DescriptorSetLayoutRef;
//
//
//    const PipelineLayoutKey& key = handle.acquire()->pipelineLayoutKey;
//    const descriptors::DescriptorSetLayoutKey& setKey = key.descriptorLayoutKeys[setID];
//    bool updateAfterBind = m_DescriptorLayouts.requires_update_after_bind(setKey);
//
//    const std::vector<descriptors::DescriptorSetLayoutKey>& descriptorLayoutKey = key.descriptorLayoutKeys;
//    std::vector<DescriptorSetLayoutRef> refs = m_DescriptorLayouts.descriptor_set_layouts(descriptorLayoutKey);
//    DescriptorSetLayoutRef layout = refs[setID];
//
//    return m_DescriptorAllocator.alloc(layout, updateAfterBind);
//}
VkDescriptorSet PipelineRegistry::allocate_descriptor_set(const DescriptorSetLayoutKey& key, std::uint32_t variableCount)
{
    bool updateAfterBind = m_DescriptorLayouts.requires_update_after_bind(key);
    vk::pipeline::DescriptorSetLayoutRef layout = m_DescriptorLayouts.descriptor_set_layout(key);
    return m_DescriptorAllocator.alloc(layout, updateAfterBind, variableCount);
}
std::vector<DescriptorSetLayoutKey> PipelineRegistry::make_descriptor_layouts(const Request& request)
{
    return m_DescriptorLayouts.make_layout_keys(request);
}
GraphicsHandle PipelineRegistry::graphics_pipeline(const Request& request)
{
    using GraphicsPipeline = vk::pipeline::GraphicsPipeline;

    PipelineKey key = make_pipeline_key(request);
    if (!m_GraphicsPipelines.contains(key))
    {
        std::lock_guard lock{ *m_pMutex };
        // In case multiple threads are waiting on the above lock
        if (!m_GraphicsPipelines.contains(key))
        {
            auto pSlot = make_graphics_slot(key, request);
            rebuild_graphics_pipeline(pSlot);
            return make_graphics_handle(pSlot);
        }
    }

    std::shared_ptr<GraphicsSlot> pSlot = get_graphics_pipeline_slot(key, request);
    return make_graphics_handle(std::move(pSlot));
}
vk::pipeline::PipelineLayoutRef PipelineRegistry::pipeline_layout(const PipelineLayoutKey& key) const
{
    return m_PipelineLayouts.pipeline_layout(key);
}
std::shared_ptr<GraphicsSlot> PipelineRegistry::make_graphics_slot(const PipelineKey& key, const Request& request)
{
    auto pSlot = std::make_shared<GraphicsSlot>();
    pSlot->request = request;
    auto [it, emplaced] = m_GraphicsPipelines.try_emplace(key, pSlot->weak_from_this());
    if (!emplaced)
    {
        it->second = pSlot->weak_from_this();
    }
    return pSlot;
}
std::shared_ptr<GraphicsResource> PipelineRegistry::make_graphics_resource(const Request& request)
{
    PipelineLayoutKey plKey = m_PipelineLayouts.make_layout_key(request, m_DescriptorLayouts);
    PipelineLayoutRef layout = m_PipelineLayouts.pipeline_layout(plKey);

    GraphicsResource resource{ .pipeline = build_graphics_pipeline(m_Device, request, layout) };
    if (request.vs)
    {
        resource.vsHash = request.vs->acquire()->hash();
    }
    if (request.fs)
    {
        resource.fsHash = request.fs->acquire()->hash();
    }
    resource.pipelineLayoutKey = std::move(plKey);

    return std::make_shared<GraphicsResource>(std::move(resource));
}
GraphicsHandle PipelineRegistry::make_graphics_handle(std::shared_ptr<GraphicsSlot> pSlot)
{
    auto cb_hot_reload = [this, slot = pSlot->shared_from_this()]() mutable
    {
        if (shader_outdated(slot))
        {
            rebuild_graphics_pipeline(slot);
        }
    };
    return GraphicsHandle{ std::move(pSlot), std::move(cb_hot_reload) };
}
std::shared_ptr<GraphicsSlot> PipelineRegistry::get_graphics_pipeline_slot(const PipelineKey& key, const Request& request)
{
    std::weak_ptr<GraphicsSlot> wpSlot = m_GraphicsPipelines.at(key);
    std::shared_ptr<GraphicsSlot> pSlot = wpSlot.lock();
    if (pSlot == nullptr)
    {
        std::lock_guard lock{ *m_pMutex };
        wpSlot = m_GraphicsPipelines.at(key);
        pSlot = wpSlot.lock();
        if (pSlot == nullptr)
        {
            pSlot = make_graphics_slot(key, request);
            rebuild_graphics_pipeline(pSlot);
        }
    }

    return pSlot;
}
PipelineKey PipelineRegistry::make_pipeline_key(const Request& request)
{
    PipelineKey key{};
    key.vs = request.vs;
    key.fs = request.fs;
    key.cs = request.cs;

    key.colorFormats = request.colorFormats;
    key.depthFormat = request.depthFormat;
    key.stencilFormat = request.stencilFormat;
    key.polygon = request.polygonMode;
    key.samples = request.MsaaSamples;

    PipelineLayoutKey plKey = m_PipelineLayouts.make_layout_key(request, m_DescriptorLayouts);
    key.pipelineLayoutHash = PipelineLayoutKeyHasher{}(plKey);

    return key;
}
void PipelineRegistry::rebuild_graphics_pipeline(std::shared_ptr<GraphicsSlot>& pSlot)
{
    std::lock_guard lock{ pSlot->initMutex };
    if (shader_outdated(pSlot))
    {
        const Request& request = pSlot->request;
        std::shared_ptr<GraphicsResource> pResource = make_graphics_resource(request);
        std::atomic_store(std::addressof(pSlot->pResource), std::move(pResource));
    }
}
}    // namespace odin::graphics::registry::pipeline
