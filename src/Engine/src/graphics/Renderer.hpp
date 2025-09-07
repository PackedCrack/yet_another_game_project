//
// Created by qwerty on 17/08/2025.
//
#pragma once

#include "ColorAttachment.hpp"
#include "ForwardPass.hpp"
#include "FrameHandler.hpp"
#include "descriptor_presets.hpp"
#include "TransferManager.hpp"
#include "registry/pipeline/PipelineRegistry.hpp"
#include "registry/resource/ResourceRegistry.hpp"
#include "vk/Allocator.hpp"
#include "vk/Device.hpp"
#include "vk/resource/DynamicUniformBuffer.hpp"
#include "vk/resource/StorageBuffer.hpp"
#include "vk/resource/IndexBuffer.hpp"
#include "vk/resource/VertexBuffer.hpp"
//
//
namespace odin::graphics
{
struct RenderResources
{
    std::reference_wrapper<const vk::resource::IndexBuffer> indexBuffer;
    std::reference_wrapper<const vk::resource::VertexBuffer> vertexBuffer;
    registry::resource::buffer::BufferHandle<vk::resource::StorageBuffer> meshTable;
};
class Renderer
{
    static constexpr std::int32_t maxInstances = 41'94304;
    static constexpr std::int32_t maxDraws = 2048;
public:
    Renderer(const std::shared_ptr<vk::Allocator>& pAllocator, vk::DeviceRef device, const FrameHandler& frameHandler);
public:
    void render_frame(const ColorAttachment& colorAttachment,
                      vk::QueueView graphicsQ,
                      const FrameContext& frameContext,
                      const TransferManager& transferManager);
    [[nodiscard]] const RenderResources render_resources() const;
private:
    void bind_global_resources(const vk::CommandBuffer& cmdBuffer) const;
    void bind_vertex_buffer(vk::CommandBufferRef cb) const;
    void bind_index_buffer(vk::CommandBufferRef cb) const;
private:
    registry::resource::ResourceRegistry m_ResourceRegistry;
    std::unique_ptr<registry::pipeline::PipelineRegistry> m_pPipelineRegistry;
    GlobalDescriptors m_GlobalDescriptors;
    IndirectDescriptors m_IndirectDescriptors;
    ForwardPass m_ForwardPass;
};
}    // namespace odin::graphics
