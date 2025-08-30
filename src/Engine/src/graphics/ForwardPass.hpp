//
// Created by qwerty on 26/07/2025.
//
#pragma once

#include "registry/pipeline/PipelineRegistry.hpp"
#include "registry/pipeline/GraphicsHandle.hpp"
#include "registry/resource/ResourceRegistry.hpp"
#include "vk/resource/DynamicUniformBuffer.hpp"
#include "vk/Allocator.hpp"
#include "vk/Device.hpp"
//
//
namespace odin::graphics
{
class ForwardPass
{
public:
    ForwardPass(const std::shared_ptr<vk::Allocator>& pAllocator,
                vk::DeviceRef device,
                registry::pipeline::PipelineRegistry& pipelineRegistry,
                registry::resource::ResourceRegistry& resourceRegistry);
public:
    void execute(vk::CommandBufferRef cmdBuffer, const VkRenderingInfo& info);
private:
    registry::pipeline::Request m_GraphicsRequest;
    registry::pipeline::GraphicsHandle m_Pipeline;
    VkDescriptorSet m_Set;
    vk::resource::DynamicUniformBuffer m_CameraBuffer;
};
}    // namespace odin::graphics
