//
// Created by qwerty on 21/09/2025.
//
#include "IndirectSetup.hpp"

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

    ShaderHandle comp = resourceRegistry.shader("indirect.comp");

    registry::pipeline::RequestBuilder builder{};
    builder = descriptors::global_preset(builder);
    builder = descriptors::indirect_preset(builder);
    builder.add_push_constant_range(0, sizeof(std::int32_t), registry::pipeline::ShaderStage::compute);
    builder.add_compute_shader(std::move(comp));

    return builder.build();
}
[[nodiscard]] VkBufferMemoryBarrier2 make_draw_count_reset_barrier(const registry::resource::buffer::BindView& view)
{
    VkBufferMemoryBarrier2 barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;
    barrier.pNext = nullptr;
    barrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
    barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
    barrier.dstStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
    barrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.buffer = view.handle;
    barrier.offset = view.offset;
    barrier.size = view.range;

    return barrier;
}
[[nodiscard]] VkBufferMemoryBarrier2 make_draw_args_barrier(FrameIndex frame, const descriptors::Indirect& indirect)
{
    VkBufferMemoryBarrier2 barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;
    barrier.pNext = nullptr;
    barrier.srcStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
    barrier.srcAccessMask = VK_ACCESS_2_SHADER_WRITE_BIT;
    barrier.dstStageMask = VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT;
    barrier.dstAccessMask = VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    BufferView view = indirect.view_draw_args(frame);
    barrier.buffer = view.handle;
    barrier.offset = view.offset;
    barrier.size = view.range;

    return barrier;
}
[[nodiscard]] VkBufferMemoryBarrier2 make_draw_count_barrier(FrameIndex frame, const descriptors::Indirect& indirect)
{
    VkBufferMemoryBarrier2 barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;
    barrier.pNext = nullptr;
    barrier.srcStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
    barrier.srcAccessMask = VK_ACCESS_2_SHADER_WRITE_BIT;
    barrier.dstStageMask = VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT;
    barrier.dstAccessMask = VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    BufferView view = indirect.view_draw_count(frame);
    barrier.buffer = view.handle;
    barrier.offset = view.offset;
    barrier.size = view.range;

    return barrier;
}
[[nodiscard]] VkBufferMemoryBarrier2 make_instance_base_barrier(FrameIndex frame, const descriptors::Indirect& indirect)
{
    VkBufferMemoryBarrier2 barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;
    barrier.pNext = nullptr;
    barrier.srcStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
    barrier.srcAccessMask = VK_ACCESS_2_SHADER_WRITE_BIT;
    barrier.dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
    barrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    BufferView view = indirect.view_instance_base(frame);
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
    barrier.srcAccessMask = VK_ACCESS_2_SHADER_WRITE_BIT;
    barrier.dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
    barrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    BufferView view = indirect.view_instance_counter(frame);
    barrier.buffer = view.handle;
    barrier.offset = view.offset;
    barrier.size = view.range;

    return barrier;
}
[[nodiscard]] VkBufferMemoryBarrier2 make_instance_index_barrier(FrameIndex frame, const descriptors::Indirect& indirect)
{
    VkBufferMemoryBarrier2 barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;
    barrier.pNext = nullptr;
    barrier.srcStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
    barrier.srcAccessMask = VK_ACCESS_2_SHADER_WRITE_BIT;
    barrier.dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
    barrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    BufferView view = indirect.view_instance_index(frame);
    barrier.buffer = view.handle;
    barrier.offset = view.offset;
    barrier.size = view.range;

    return barrier;
}
[[nodiscard]] VkBufferMemoryBarrier2 make_instance_info_barrier(FrameIndex frame, const descriptors::Indirect& indirect)
{
    VkBufferMemoryBarrier2 barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;
    barrier.pNext = nullptr;
    barrier.srcStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
    barrier.srcAccessMask = VK_ACCESS_2_SHADER_WRITE_BIT;
    barrier.dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
    barrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    BufferView view = indirect.view_instance_info(frame);
    barrier.buffer = view.handle;
    barrier.offset = view.offset;
    barrier.size = view.range;

    return barrier;
}
[[nodiscard]] std::tuple<VkDependencyInfo, std::array<VkBufferMemoryBarrier2, 6>> make_indirect_stage_barriers(FrameIndex frame, const descriptors::Indirect& indirect)
{
    std::array<VkBufferMemoryBarrier2, 6> barriers{};
    barriers[0] = make_draw_args_barrier(frame, indirect);
    barriers[1] = make_draw_count_barrier(frame, indirect);
    barriers[2] = make_instance_base_barrier(frame, indirect);
    barriers[3] = make_instance_counter_barrier(frame, indirect);
    barriers[4] = make_instance_index_barrier(frame, indirect);
    barriers[5] = make_instance_info_barrier(frame, indirect);

    VkDependencyInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    info.pNext = nullptr;
    info.dependencyFlags = VK_NO_FLAGS;
    info.bufferMemoryBarrierCount = static_cast<std::uint32_t>(barriers.size());
    info.pBufferMemoryBarriers = barriers.data();

    return { info, barriers };
}
void reset_draw_count(vk::CommandBufferRef cmd, FrameIndex frame, const descriptors::Indirect& indirect)
{
    BufferView view = indirect.view_draw_count(frame);
    vkCmdFillBuffer(cmd.handle, view.handle, view.offset, view.range, 0);

    VkBufferMemoryBarrier2 barrier = make_draw_count_reset_barrier(view);
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
IndirectSetup::IndirectSetup(registry::pipeline::PipelineRegistry& pipelineRegistry, registry::resource::ResourceRegistry& resourceRegistry)
    : PipelineResolver<IndirectSetup>{ pipelineRegistry }
    , m_Request{ make_request(resourceRegistry) }
    , m_Pipeline{ pipelineRegistry.compute_pipeline(m_Request) }
{}
void IndirectSetup::execute(const FrameContext& frameContext,
                            const descriptors::Global& global,
                            const descriptors::Indirect& indirect,
                            std::int32_t invocationCount) const
{
    vk::CommandBufferRef cmdBuffer = frameContext.graphicsBuffer.get().handle();

    m_Pipeline.acquire()->pipeline.bind(cmdBuffer);
    bind_descriptors(frameContext, global, indirect);

    reset_draw_count(cmdBuffer, frameContext.frame, indirect);

    push_invocation_count(cmdBuffer, invocationCount);
    
    std::uint32_t maxWorkgroupSize = vk::PhysicalDevice::properties().max_compute_work_group_count_x();
    vkCmdDispatch(cmdBuffer.handle, maxWorkgroupSize, 1, 1);

    auto[dependency, barriers] = make_indirect_stage_barriers(frameContext.frame, indirect);
    vkCmdPipelineBarrier2(cmdBuffer.handle, std::addressof(dependency));
}
void IndirectSetup::bind_descriptors(const FrameContext& frameContext,
                                     const descriptors::Global& global,
                                     const descriptors::Indirect& indirect) const
{
    vk::pipeline::PipelineLayoutRef layout = resolve_layout(m_Pipeline);

    vk::CommandBufferRef cmdBuffer = frameContext.graphicsBuffer.get().handle();
    global.bind(cmdBuffer, layout, VK_SHADER_STAGE_COMPUTE_BIT);
    indirect.bind(cmdBuffer, layout, VK_SHADER_STAGE_COMPUTE_BIT, frameContext.frame);
}
void IndirectSetup::push_invocation_count(vk::CommandBufferRef cmd, std::int32_t invocationCount) const
{
    vk::pipeline::PipelineLayoutRef layout = resolve_layout(m_Pipeline);
    vkCmdPushConstants(cmd.handle,
                       layout.handle,
                       VK_SHADER_STAGE_COMPUTE_BIT,
                       0,
                       sizeof(std::int32_t),
                       std::addressof(invocationCount));
}
}    // namespace odin::graphics::pass
