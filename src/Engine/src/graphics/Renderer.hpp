//
// Created by qwerty on 17/08/2025.
//
#pragma once

#include "Presenter.hpp"
#include "ColorAttachment.hpp"
#include "FrameHandler.hpp"
#include "TransferManager.hpp"
#include "descriptors/Global.hpp"
#include "descriptors/Indirect.hpp"
#include "pass/Forward.hpp"
#include "pass/FrustumCull.hpp"
#include "pass/IndirectSetup.hpp"
#include "pass/InstanceCompaction.hpp"
#include "vk/Allocator.hpp"
#include "vk/Device.hpp"
#include "registry/pipeline/PipelineRegistry.hpp"
#include "registry/resource/ResourceRegistry.hpp"
//
//
namespace odin::graphics
{
class Renderer
{
public:
    Renderer(vk::DeviceRef device,
             const Presenter& presenter,
             registry::resource::ResourceRegistry& resourceRegistry,
             registry::pipeline::PipelineRegistry& pipelineRegistry);
public:
    void render_frame(const ColorAttachment& colorAttachment,
                      vk::QueueView graphicsQ,
                      const FrameContext& frameContext,
                      const TransferManager& transferManager,
                      registry::resource::ResourceRegistry& resourceRegistry,
                      std::int32_t instanceCount);
private:
    void bind_vertex_buffer(vk::CommandBufferRef cb, const registry::resource::ResourceRegistry& resourceRegistry) const;
    void bind_index_buffer(vk::CommandBufferRef cb, const registry::resource::ResourceRegistry& resourceRegistry) const;
private:
    descriptors::Global m_Global;
    descriptors::Indirect m_Indirect;
    pass::Forward m_Forward;
    pass::FrustumCull m_FrustumCull;
    pass::IndirectSetup m_IndirectSetup;
    pass::InstanceCompaction m_InstanceCompaction;
};
}    // namespace odin::graphics
