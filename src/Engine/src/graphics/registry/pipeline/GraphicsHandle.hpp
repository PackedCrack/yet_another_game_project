//
// Created by qwerty on 26/07/2025.
//
#pragma once

#include "GraphicsSlot.hpp"
#include "../Handle.hpp"
#include "../../vk/pipeline/GraphicsPipeline.hpp"
//
//
namespace odin::graphics::registry::pipeline
{
struct GraphicsHandle : public Handle<GraphicsHandle, GraphicsSlot, GraphicsResource>
{
    GraphicsHandle(std::shared_ptr<GraphicsSlot> pSlot);
};
}    // namespace odin::graphics::registry::pipeline
