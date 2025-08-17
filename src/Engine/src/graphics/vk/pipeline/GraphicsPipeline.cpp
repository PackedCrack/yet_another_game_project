//
// Created by qwerty on 17/08/2025.
//
#include "GraphicsPipeline.hpp"

//
//
namespace odin::graphics::vk::pipeline
{
GraphicsPipeline::GraphicsPipeline(DeviceRef device, const VkGraphicsPipelineCreateInfo& info)
	: Pipeline<GraphicsPipeline>{ device,info }
{}
VkPipelineBindPoint GraphicsPipeline::bind_point() const
{
	return VK_PIPELINE_BIND_POINT_GRAPHICS;
}
GraphicsPipelineRef GraphicsPipeline::handle() const
{
	ODIN_ASSERT(m_Pipeline != VK_NULL_HANDLE);
	return GraphicsPipelineRef{ .handle = m_Pipeline };
}
VkPipeline GraphicsPipeline::make_pipeline(DeviceRef device, const VkGraphicsPipelineCreateInfo& info) const
{
	VkPipeline pipeline{};
	VK_CHECK(vkCreateGraphicsPipelines(device.handle, VK_NULL_HANDLE, 1, std::addressof(info), nullptr, std::addressof(pipeline)),
			 "Failed to create Vulkan Graphics Pipeline.");

	return pipeline;
}
} // namespace odin::graphics::vk::pipeline