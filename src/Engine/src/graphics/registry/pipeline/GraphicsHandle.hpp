//
// Created by qwerty on 26/08/2025.
//
#pragma once

#include "GraphicsSlot.hpp"
#include "PipelineHandle.hpp"
#include "../../vk/pipeline/GraphicsPipeline.hpp"
//
//
namespace odin::graphics::registry::pipeline
{
struct GraphicsHandle : public PipelineHandle<GraphicsHandle, GraphicsSlot, GraphicsResource>
{
    GraphicsHandle(std::shared_ptr<GraphicsSlot> pSlot, std::function<void()> hot_reload);
};
}    // namespace odin::graphics::registry::pipeline
