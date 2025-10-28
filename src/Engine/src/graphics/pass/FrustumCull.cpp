//
// Created by qwerty on 28/10/2025.
//
#include "FrustumCull.hpp"

#include "../gpu_types.hpp"
#include "../registry/pipeline/Request.hpp"
#include "../registry/pipeline/RequestBuilder.hpp"
#include "../registry/resource/shader/ShaderHandle.hpp"
#include "../vk/pipeline/DescriptorWriter.hpp"
#include "../vk/PhysicalDevice.hpp"
//
//
namespace
{
using namespace odin::graphics;
using namespace odin::graphics::pass;
using BufferView = registry::resource::buffer::BindView;
//
//
[[nodiscard]] registry::pipeline::Request make_request(registry::resource::ResourceRegistry& resourceRegistry)
{
    using ShaderHandle = registry::resource::shader::ShaderHandle;

    ShaderHandle comp = resourceRegistry.shader("frustum_cull.comp");

    registry::pipeline::RequestBuilder builder{};
    builder = descriptors::indirect_preset(builder);
    builder.add_push_constant_range(0, sizeof(std::int32_t), registry::pipeline::ShaderStage::compute);
    builder.add_compute_shader(std::move(comp));

    return builder.build();
}
[[nodiscard]] VkBufferMemoryBarrier2 make_instance_counter_reset_barrier(const registry::resource::buffer::BindView& view)
{
    VkBufferMemoryBarrier2 barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;
    barrier.pNext = nullptr;
    barrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
    barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
    barrier.dstStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
    barrier.dstAccessMask = VK_ACCESS_2_SHADER_STORAGE_READ_BIT | VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.buffer = view.handle;
    barrier.offset = view.offset;
    barrier.size = view.range;

    return barrier;
}
[[nodiscard]] VkBufferMemoryBarrier2 make_instance_counter_barrier(FrameIndex frame, const descriptors::Indirect& indirect)
{
    VkBufferMemoryBarrier2 barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;
    barrier.pNext = nullptr;
    barrier.srcStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
    barrier.srcAccessMask = VK_ACCESS_2_SHADER_STORAGE_READ_BIT | VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT;
    barrier.dstStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
    barrier.dstAccessMask = VK_ACCESS_2_SHADER_STORAGE_READ_BIT;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    BufferView view = indirect.view_instance_counter(frame);
    barrier.buffer = view.handle;
    barrier.offset = view.offset;
    barrier.size = view.range;

    return barrier;
}
void reset_instance_counter(vk::CommandBufferRef cmd, FrameIndex frame, const descriptors::Indirect& indirect)
{
    BufferView view = indirect.view_instance_counter(frame);
    vkCmdFillBuffer(cmd.handle, view.handle, view.offset, view.range, 0);

    VkBufferMemoryBarrier2 barrier = make_instance_counter_reset_barrier(view);
    VkDependencyInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    info.pNext = nullptr;
    info.dependencyFlags = VK_NO_FLAGS;
    info.bufferMemoryBarrierCount = 1;
    info.pBufferMemoryBarriers = std::addressof(barrier);

    vkCmdPipelineBarrier2(cmd.handle, std::addressof(info));
}
}    // namespace
namespace odin::graphics::pass
{
FrustumCull::FrustumCull(registry::pipeline::PipelineRegistry& pipelineRegistry, registry::resource::ResourceRegistry& resourceRegistry)
    : PipelineResolver<FrustumCull>{ pipelineRegistry }
    , m_Request{ make_request(resourceRegistry) }
    , m_Pipeline{ pipelineRegistry.compute_pipeline(m_Request) }
{}
void FrustumCull::execute(const FrameContext& frameContext,
                          const descriptors::Indirect& indirect,
                          std::int32_t instanceCount) const
{
    vk::CommandBufferRef cmdBuffer = frameContext.graphicsBuffer.get().handle();

    m_Pipeline.acquire()->pipeline.bind(cmdBuffer);
    bind_descriptors(frameContext, indirect);

    reset_instance_counter(cmdBuffer, frameContext.frame, indirect);

    push_instance_count(cmdBuffer, instanceCount);

    std::uint32_t maxWorkgroupCount = vk::PhysicalDevice::properties().max_compute_work_group_count_x();
    vkCmdDispatch(cmdBuffer.handle, maxWorkgroupCount, 1, 1);


    VkBufferMemoryBarrier2 barrier = make_instance_counter_barrier(frameContext.frame, indirect);
    VkDependencyInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    info.pNext = nullptr;
    info.dependencyFlags = VK_NO_FLAGS;
    info.bufferMemoryBarrierCount = 1;
    info.pBufferMemoryBarriers = std::addressof(barrier);

    vkCmdPipelineBarrier2(cmdBuffer.handle, std::addressof(info));
}
void FrustumCull::bind_descriptors(const FrameContext& frameContext,
                                   const descriptors::Indirect& indirect) const
{
    vk::pipeline::PipelineLayoutRef layout = resolve_layout(m_Pipeline);

    vk::CommandBufferRef cmdBuffer = frameContext.graphicsBuffer.get().handle();
    indirect.bind(cmdBuffer, layout, VK_SHADER_STAGE_COMPUTE_BIT, frameContext.frame);
}
void FrustumCull::push_instance_count(vk::CommandBufferRef cmd, std::int32_t instanceCount) const
{
    vk::pipeline::PipelineLayoutRef layout = resolve_layout(m_Pipeline);
    vkCmdPushConstants(cmd.handle, layout.handle, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(std::int32_t), std::addressof(instanceCount));
}
}    // namespace odin::graphics::pass
