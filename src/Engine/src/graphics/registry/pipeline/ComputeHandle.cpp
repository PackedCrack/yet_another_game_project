//
// Created by qwerty on 21/09/2025.
//
#include "ComputeHandle.hpp"
namespace odin::graphics::registry::pipeline
{
ComputeHandle::ComputeHandle(std::shared_ptr<ComputeSlot> pSlot, std::function<void()> hot_reload)
    : PipelineHandle<ComputeHandle, ComputeSlot, ComputeResource>(std::move(pSlot), std::move(hot_reload))
{}
}    // namespace odin::graphics::registry::pipeline
