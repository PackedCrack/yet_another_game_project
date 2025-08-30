//
// Created by qwerty on 26/08/2025.
//
#pragma once

#include "Request.hpp"
#include "PipelineLayoutKey.hpp"
#include "../Slot.hpp"
#include "../../vk/pipeline/GraphicsPipeline.hpp"
#include "../../vk/pipeline/PipelineLayout.hpp"
//
//
namespace odin::graphics::registry::pipeline
{
struct GraphicsResource
{
    vk::pipeline::GraphicsPipeline pipeline;
    PipelineLayoutKey pipelineLayoutKey;
    std::optional<std::uint64_t> vsHash;
    std::optional<std::uint64_t> fsHash;
    std::optional<std::uint64_t> csHash;
};
struct GraphicsSlot : public Slot<GraphicsSlot, GraphicsResource>
{
    Request request;
};
}    // namespace odin::graphics::registry::pipeline
