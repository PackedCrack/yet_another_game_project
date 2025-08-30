//
// Created by qwerty on 26/08/2025.
//
#include "GraphicsHandle.hpp"
//
//
namespace odin::graphics::registry::pipeline
{
GraphicsHandle::GraphicsHandle(std::shared_ptr<GraphicsSlot> pSlot, std::function<void()> hot_reload)
    : PipelineHandle<GraphicsHandle, GraphicsSlot, GraphicsResource>(std::move(pSlot), std::move(hot_reload))
{}
}    // namespace odin::graphics::registry::pipeline
