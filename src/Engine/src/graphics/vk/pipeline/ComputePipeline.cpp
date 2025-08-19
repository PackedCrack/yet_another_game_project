//
// Created by qwerty on 17/08/2025.
//
#include "ComputePipeline.hpp"
//
//
namespace odin::graphics::vk::pipeline
{
ComputePipeline::ComputePipeline(DeviceRef device, const VkComputePipelineCreateInfo& info)
    : Pipeline<ComputePipeline>{ device, info }
{}
VkPipelineBindPoint ComputePipeline::bind_point() const
{
    return VK_PIPELINE_BIND_POINT_COMPUTE;
}
ComputePipelineRef ComputePipeline::handle() const
{
    ODIN_ASSERT(m_Pipeline != VK_NULL_HANDLE);
    return ComputePipelineRef{ .handle = m_Pipeline };
}
VkPipeline ComputePipeline::make_pipeline(DeviceRef device, const VkComputePipelineCreateInfo& info) const
{
    VkPipeline pipeline{};
    VK_CHECK(vkCreateComputePipelines(device.handle, VK_NULL_HANDLE, 1, std::addressof(info), nullptr, std::addressof(pipeline)),
             "Failed to create Vulkan Compute Pipeline.");

    return pipeline;
}
}    // namespace odin::graphics::vk::pipeline
