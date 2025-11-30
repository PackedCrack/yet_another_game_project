//
// Created by qwerty on 21/09/2025.
//
#pragma once

#include "Request.hpp"
#include "PipelineLayoutKey.hpp"
#include "../Slot.hpp"
#include "../../vk/pipeline/ComputePipeline.hpp"
#include "../../vk/pipeline/PipelineLayout.hpp"
//
//
namespace odin::graphics::registry::pipeline
{
struct ComputeResource
{
    vk::pipeline::ComputePipeline pipeline;
    PipelineLayoutKey pipelineLayoutKey;
    std::optional<std::uint64_t> csHash;
};
struct ComputeSlot : public Slot<ComputeSlot, ComputeResource>
{
    Request request;
};
}    // namespace odin::graphics::registry::pipeline
