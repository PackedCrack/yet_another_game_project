//
// Created by qwerty on 17/08/2025.
//
#pragma once

#include "ColorAttachment.hpp"
#include "FrameHandler.hpp"
#include "TransferManager.hpp"
#include "descriptors/Global.hpp"
#include "descriptors/Indirect.hpp"
#include "pass/Forward.hpp"
#include "pass/IndirectSetup.hpp"
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
    // TODO: I pulled these numbers from my ass
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
    void bind_vertex_buffer(vk::CommandBufferRef cb) const;
    void bind_index_buffer(vk::CommandBufferRef cb) const;
private:
    registry::resource::ResourceRegistry m_ResourceRegistry;
    std::unique_ptr<registry::pipeline::PipelineRegistry> m_pPipelineRegistry;
    descriptors::Global m_Global;
    descriptors::Indirect m_Indirect;
    pass::Forward m_Forward;
    pass::IndirectSetup m_IndirectSetup;
};
}    // namespace odin::graphics
