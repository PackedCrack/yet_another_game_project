//
// Created by qwerty on 17/08/2025.
//
#include "Renderer.hpp"

#include "gpu_types.hpp"
#include "vk/vulkan_defines.hpp"
//
//
namespace
{
using RenderResources = odin::graphics::RenderResources;
using TransferEpoch = odin::graphics::TransferEpoch;
using Allocator = odin::graphics::vk::Allocator;
using CommandBuffer = odin::graphics::vk::CommandBuffer;
using CommandBufferRef = odin::graphics::vk::CommandBufferRef;
using FenceRef = odin::graphics::vk::synchronization::FenceRef;
using QueueView = odin::graphics::vk::QueueView;
using SemaphoreRef = odin::graphics::vk::synchronization::SemaphoreRef;
using TimelineSemaphoreRef = odin::graphics::vk::synchronization::TimelineSemaphoreRef;
//
//
VkSemaphoreSubmitInfo submit_info_semaphore(VkSemaphore semaphore, std::uint64_t value, VkPipelineStageFlags2 flags)
{
    return VkSemaphoreSubmitInfo{ .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
                                  .pNext = nullptr,
                                  .semaphore = semaphore,
                                  .value = value,
                                  .stageMask = flags,
                                  .deviceIndex = 0 };
}
// clang-format off
template<typename... stage_mask_t>
requires(std::same_as<VkPipelineStageFlags2, std::remove_cvref_t<stage_mask_t>> && ...)
[[nodiscard]] 
VkSemaphoreSubmitInfo submit_info_timeline_semaphore(TimelineSemaphoreRef semaphore, std::uint64_t value, stage_mask_t&&... masks)
{
    static_assert((sizeof(masks) + ...) > 0);

    VkPipelineStageFlags2 flags = (masks | ...);
    return submit_info_semaphore(semaphore.handle, value, flags);
}
template<typename... stage_mask_t>
requires(std::same_as<VkPipelineStageFlags2, std::remove_cvref_t<stage_mask_t>> && ...)
[[nodiscard]] 
VkSemaphoreSubmitInfo submit_info_binary_semaphore(SemaphoreRef semaphore, VkPipelineStageFlags2 mask, stage_mask_t&&... masks)
{
    VkPipelineStageFlags2 flags = (mask | ... | masks);
    return submit_info_semaphore(semaphore.handle, 0, flags);
}
// clang-format on
VkCommandBufferSubmitInfo submit_info_cmd_buffer(CommandBufferRef cmdBuffer)
{
    return VkCommandBufferSubmitInfo{ .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
                                      .pNext = nullptr,
                                      .commandBuffer = cmdBuffer.handle,
                                      .deviceMask = 0 };
}
void submit(QueueView queue,
            const CommandBuffer& cmdBuffer,
            SemaphoreRef colorAttachmentReady,
            SemaphoreRef graphicsFinished,
            FenceRef inFlight,
            const std::optional<TransferEpoch>& transferEpoch)
{
    CommandBufferRef cb = cmdBuffer.handle();

    VkCommandBufferSubmitInfo cbInfo = submit_info_cmd_buffer(cb);

    std::vector<VkSemaphoreSubmitInfo> waitSemaphores{};
    if (transferEpoch)
    {
        TimelineSemaphoreRef transferSem = transferEpoch->semaphore;
        std::uint64_t waitValue = transferEpoch->waitValue;
        waitSemaphores.push_back(submit_info_timeline_semaphore(transferSem, waitValue, VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT));
    }
    VkSemaphoreSubmitInfo colorAttachReadyInfo =
        submit_info_binary_semaphore(colorAttachmentReady, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT);
    waitSemaphores.push_back(colorAttachReadyInfo);

    VkSemaphoreSubmitInfo graphicsFinishedInfo = submit_info_binary_semaphore(graphicsFinished, VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT);

    VkSubmitInfo2 submitInfo = { .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
                                 .pNext = nullptr,
                                 .flags = VK_NO_FLAGS,
                                 .waitSemaphoreInfoCount = static_cast<std::uint32_t>(waitSemaphores.size()),
                                 .pWaitSemaphoreInfos = waitSemaphores.data(),
                                 .commandBufferInfoCount = 1,
                                 .pCommandBufferInfos = std::addressof(cbInfo),
                                 .signalSemaphoreInfoCount = 1,
                                 .pSignalSemaphoreInfos = std::addressof(graphicsFinishedInfo) };

    VK_CHECK(vkQueueSubmit2(queue.handle, 1, std::addressof(submitInfo), inFlight.handle), "Failed to submit to Graphics Queue!");
}
}    // namespace
namespace odin::graphics
{
Renderer::Renderer(const std::shared_ptr<vk::Allocator>& pAllocator, vk::DeviceRef device, const FrameHandler& frameHandler)
    : m_ResourceRegistry{ device, pAllocator, frameHandler, maxDraws, maxInstances }
    , m_pPipelineRegistry{ registry::pipeline::PipelineRegistry::make(device) }
    , m_Global{ device, m_ResourceRegistry, *m_pPipelineRegistry }
    , m_Indirect{ device, m_ResourceRegistry, *m_pPipelineRegistry }
    , m_Forward{ *m_pPipelineRegistry, m_ResourceRegistry }
    , m_FrustumCull{ *m_pPipelineRegistry, m_ResourceRegistry }
    , m_IndirectSetup{ *m_pPipelineRegistry, m_ResourceRegistry }
    , m_InstanceCompaction{ *m_pPipelineRegistry, m_ResourceRegistry }
{}
void Renderer::render_frame(const ColorAttachment& colorAttachment,
                            vk::QueueView graphicsQ,
                            const FrameContext& frameContext,
                            const TransferManager& transferManager)
{
    vk::CommandBuffer& gfxCmdBuffer = frameContext.graphicsBuffer.get();
    // Reset cmdBuffer and prepare it for commands
    gfxCmdBuffer.reset();
    gfxCmdBuffer.begin();

    // Acquire Transfer buffers
    transferManager.record_buffer_acquisition(graphicsQ, gfxCmdBuffer.handle());

    // Bind Geometry buffers
    vk::CommandBufferRef cb = gfxCmdBuffer.handle();
    bind_vertex_buffer(cb);
    bind_index_buffer(cb);


    // execute all passes - todo render graph in the future
    std::int32_t instanceCount = 512;    // TODO: Get this from total entities..?
    m_FrustumCull.execute(frameContext, m_Global, m_Indirect, instanceCount);

    m_IndirectSetup.execute(frameContext, m_Global, m_Indirect, instanceCount);

    m_InstanceCompaction.execute(frameContext, m_Global, m_Indirect, instanceCount);

    m_Forward.execute(frameContext, colorAttachment, m_Global, m_Indirect);


    // Should present be its own pass?
    VkImageMemoryBarrier2 presentBarrier = colorAttachment.barrier_to_present();
    const VkDependencyInfo dep2{ .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                                 .pNext = nullptr,
                                 .dependencyFlags = 0,
                                 .memoryBarrierCount = 0,
                                 .pMemoryBarriers = nullptr,
                                 .bufferMemoryBarrierCount = 0,
                                 .pBufferMemoryBarriers = nullptr,
                                 .imageMemoryBarrierCount = 1,
                                 .pImageMemoryBarriers = std::addressof(presentBarrier) };
    vkCmdPipelineBarrier2(gfxCmdBuffer.handle().handle, std::addressof(dep2));


    // End rendering
    gfxCmdBuffer.end();


    // Submit
    std::optional<TransferEpoch> transferEpoch = transferManager.epoch();
    submit(graphicsQ, gfxCmdBuffer, frameContext.colorAttachmentReady, frameContext.graphicsFinished, frameContext.inFlight, transferEpoch);
}
const RenderResources Renderer::render_resources() const
{
    return RenderResources{ .indexBuffer = m_ResourceRegistry.index_buffer(),
                            .vertexBuffer = m_ResourceRegistry.vertex_buffer(),
                            .meshTable = m_ResourceRegistry.storage_buffer(m_ResourceRegistry.SSBO_MESH_TABLE) };
}
void Renderer::bind_vertex_buffer(vk::CommandBufferRef cb) const
{
    using namespace vk::resource;

    const VertexBuffer& vb = m_ResourceRegistry.vertex_buffer();
    BufferRef vbRef = vb.handle();
    VkDeviceSize offset = 0;
    VkDeviceSize size = vb.byte_capacity();
    VkDeviceSize stride = sizeof(VertexBuffer::vertex_t);
    vkCmdBindVertexBuffers2(cb.handle,
                            0,
                            1,
                            std::addressof(vbRef.handle),
                            std::addressof(offset),
                            std::addressof(size),
                            std::addressof(stride));
}
void Renderer::bind_index_buffer(vk::CommandBufferRef cb) const
{
    using namespace vk::resource;

    const IndexBuffer& ib = m_ResourceRegistry.index_buffer();
    BufferRef ibRef = ib.handle();
    vkCmdBindIndexBuffer2(cb.handle, ibRef.handle, 0, VK_WHOLE_SIZE, VK_INDEX_TYPE_UINT16);
}
}    // namespace odin::graphics
