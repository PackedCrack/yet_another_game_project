//
// Created by qwerty on 17/08/2025.
//
#pragma once

#include "ColorAttachment.hpp"
#include "FrameHandler.hpp"
#include "TransferManager.hpp"
#include "vk/Allocator.hpp"
#include "vk/resource/StorageBuffer.hpp"
#include "vk/resource/IndexBuffer.hpp"
#include "vk/resource/VertexBuffer.hpp"
//
//
namespace odin::graphics
{
struct RenderResources
{
    vk::resource::StorageBuffer meshTable;
    vk::resource::IndexBuffer indexBuffer;
    vk::resource::VertexBuffer vertexBuffer;
};
class Renderer
{
public:
    Renderer(const std::shared_ptr<vk::Allocator>& pAllocator);
public:
    void render_frame(const ColorAttachment& colorAttachment,
                      vk::QueueView graphicsQ,
                      const FrameContext& frameContext,
                      const TransferManager& transferManager);
    [[nodiscard]] const RenderResources& render_resources() const;
private:
    void bind_global_resources(const vk::CommandBuffer& cmdBuffer) const;
    void bind_vertex_buffer(vk::CommandBufferRef cb) const;
    void bind_index_buffer(vk::CommandBufferRef cb) const;
private:
    RenderResources m_RenderResources;
};
}    // namespace odin::graphics
