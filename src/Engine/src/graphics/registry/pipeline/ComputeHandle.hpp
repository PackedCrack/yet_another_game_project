//
// Created by qwerty on 21/09/2025.
//
#pragma once

#include "ComputeSlot.hpp"
#include "PipelineHandle.hpp"
#include "../../vk/pipeline/GraphicsPipeline.hpp"
//
//
namespace odin::graphics::registry::pipeline
{
struct ComputeHandle : public PipelineHandle<ComputeHandle, ComputeSlot, ComputeResource>
{
    ComputeHandle(std::shared_ptr<ComputeSlot> pSlot, std::function<void()> hot_reload);
};
}    // namespace odin::graphics::registry::pipeline
