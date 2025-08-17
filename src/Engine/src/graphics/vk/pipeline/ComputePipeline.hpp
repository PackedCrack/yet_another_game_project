//
// Created by qwerty on 17/08/2025.
//
#pragma once

#include "Pipeline.hpp"
//
//
namespace odin::graphics::vk::pipeline
{
struct ComputePipelineRef
{
	VkPipeline handle;
};
class ComputePipeline : public Pipeline<ComputePipeline>
{
	friend class Pipeline<ComputePipeline>;
public:
	ComputePipeline(DeviceRef device, const VkComputePipelineCreateInfo& info);
	[[nodiscard]] VkPipelineBindPoint bind_point() const;
	[[nodiscard]] ComputePipelineRef handle() const;
private:
	[[nodiscard]] VkPipeline make_pipeline(DeviceRef device, const VkComputePipelineCreateInfo& info) const;
};
} // namespace odin::graphics::vk::pipeline