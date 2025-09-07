//
// Created by qwerty on 26/07/2025.
//
#pragma once

#include "../descriptors/Global.hpp"
#include "../descriptors/Indirect.hpp"
#include "../ColorAttachment.hpp"
#include "../registry/pipeline/PipelineRegistry.hpp"
#include "../registry/pipeline/GraphicsHandle.hpp"
#include "../registry/resource/ResourceRegistry.hpp"
#include "../vk/resource/DynamicUniformBuffer.hpp"
#include "../vk/Allocator.hpp"
#include "../vk/Device.hpp"
//
//
namespace odin::graphics::pass
{
class Forward
{
public:
    Forward(registry::pipeline::PipelineRegistry& pipelineRegistry, registry::resource::ResourceRegistry& resourceRegistry);
public:
    void execute(const FrameContext& frameContext,
                 const ColorAttachment& colorAttachment,
                 const descriptors::Global& global,
                 const descriptors::Indirect& indirect) const;
private:
    void bind_descriptors(const FrameContext& frameContext, const descriptors::Global& global, const descriptors::Indirect& indirect) const;
private:
    registry::pipeline::Request m_GraphicsRequest;
    registry::pipeline::GraphicsHandle m_Pipeline;
    std::reference_wrapper<const registry::pipeline::PipelineRegistry> m_PipelineRegistry;
};
}    // namespace odin::graphics::pass
