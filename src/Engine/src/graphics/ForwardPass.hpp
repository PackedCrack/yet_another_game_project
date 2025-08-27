//
// Created by qwerty on 26/07/2025.
//
#pragma once

#include "registry/pipeline/PipelineRegistry.hpp"
#include "registry/pipeline/GraphicsHandle.hpp"
#include "registry/resource/ResourceRegistry.hpp"
//
//
namespace odin::graphics
{
class ForwardPass
{
public:
    ForwardPass(registry::pipeline::PipelineRegistry& pipelineRegistry, registry::resource::ResourceRegistry& resourceRegistry);
private:
    registry::pipeline::Request m_PipelineRequest;
    registry::pipeline::GraphicsHandle m_Pipeline;
};
}    // namespace odin::graphics
