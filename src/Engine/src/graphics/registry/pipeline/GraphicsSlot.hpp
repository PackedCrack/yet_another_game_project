//
// Created by qwerty on 26/07/2025.
//
#pragma once

#include "../Slot.hpp"
#include "../../vk/pipeline/GraphicsPipeline.hpp"
//
//
namespace odin::graphics::registry::pipeline
{
struct GraphicsSlot : public Slot<GraphicsSlot, vk::pipeline::GraphicsPipeline>
{};
}    // namespace odin::graphics::registry::pipeline
