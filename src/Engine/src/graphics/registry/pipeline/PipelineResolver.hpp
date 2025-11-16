//
// Created by qwerty on 19/10/2025.
//
//
// This is shared between cpp and glsl
//
#pragma once

#include "PipelineRegistry.hpp"
#include "ComputeHandle.hpp"
#include "GraphicsHandle.hpp"
//
//
namespace odin::graphics::registry::pipeline
{
template<typename derived_t>
class PipelineResolver
{
public:
    PipelineResolver(PipelineRegistry& registry)
        : m_Registry{ registry }
    {}
public:
    [[nodiscard]] vk::pipeline::PipelineLayoutRef resolve_layout(const GraphicsHandle& pipeline) const
    {
        const PipelineLayoutKey& key = pipeline.acquire()->pipelineLayoutKey;
        return m_Registry.get().pipeline_layout(key);
    }
    [[nodiscard]] vk::pipeline::PipelineLayoutRef resolve_layout(const ComputeHandle& pipeline) const
    {
        const PipelineLayoutKey& key = pipeline.acquire()->pipelineLayoutKey;
        return m_Registry.get().pipeline_layout(key);
    }
private:
    std::reference_wrapper<const PipelineRegistry> m_Registry;
};
}    // namespace odin::graphics::registry::pipeline
