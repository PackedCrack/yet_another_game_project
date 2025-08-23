//
// Created by qwerty on 17/08/2025.
//
#pragma once

#include "Pipeline.hpp"
//
//
namespace odin::graphics::vk::pipeline
{
struct GraphicsPipelineRef
{
    VkPipeline handle;
};
class GraphicsPipeline : public Pipeline<GraphicsPipeline>
{
    friend class Pipeline<GraphicsPipeline>;
public:
    GraphicsPipeline(DeviceRef device, const VkGraphicsPipelineCreateInfo& info);
    [[nodiscard]] VkPipelineBindPoint bind_point() const;
    [[nodiscard]] GraphicsPipelineRef handle() const;
private:
    [[nodiscard]] VkPipeline make_pipeline(DeviceRef device, const VkGraphicsPipelineCreateInfo& info) const;
};
}    // namespace odin::graphics::vk::pipeline
