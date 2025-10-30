//
// Created by qwerty on 21/09/2025.
//
#pragma once

#include "../descriptors/Global.hpp"
#include "../descriptors/Indirect.hpp"
#include "../registry/pipeline/PipelineRegistry.hpp"
#include "../registry/pipeline/ComputeHandle.hpp"
#include "../registry/resource/ResourceRegistry.hpp"
#include "../registry/pipeline/PipelineResolver.hpp"
//
//
namespace odin::graphics::pass
{
class IndirectSetup : public registry::pipeline::PipelineResolver<IndirectSetup>
{
public:
    IndirectSetup(registry::pipeline::PipelineRegistry& pipelineRegistry, registry::resource::ResourceRegistry& resourceRegistry);
public:
    void execute(const FrameContext& frameContext,
                 const descriptors::Global& global,
                 const descriptors::Indirect& indirect,
                 std::int32_t meshCount) const;
private:
    void bind_descriptors(const FrameContext& frameContext, const descriptors::Global& global, const descriptors::Indirect& indirect) const;
    void push_mesh_count(vk::CommandBufferRef cmd, std::int32_t meshCount) const;
private:
    registry::pipeline::Request m_Request;
    registry::pipeline::ComputeHandle m_Pipeline;
};
}    // namespace odin::graphics::pass
