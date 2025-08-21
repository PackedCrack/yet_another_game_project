#include "Renderer.hpp"

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
template<typename... stage_mask_t>
requires(std::same_as<VkPipelineStageFlags2, std::remove_cvref_t<stage_mask_t>> && ...)
[[nodiscard]] VkSemaphoreSubmitInfo
submit_info_timeline_semaphore(TimelineSemaphoreRef semaphore, std::uint64_t value, stage_mask_t&&... masks)
{
    static_assert((sizeof(masks) + ...) > 0);

    VkPipelineStageFlags2 flags = (masks | ...);
    return submit_info_semaphore(semaphore.handle, value, flags);
}
template<typename... stage_mask_t>
requires(std::same_as<VkPipelineStageFlags2, std::remove_cvref_t<stage_mask_t>> && ...)
[[nodiscard]] VkSemaphoreSubmitInfo
submit_info_binary_semaphore(SemaphoreRef semaphore, VkPipelineStageFlags2 mask, stage_mask_t&&... masks)
{
    VkPipelineStageFlags2 flags = (mask | ... | masks);
    return submit_info_semaphore(semaphore.handle, 0, flags);
}
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
[[nodiscard]] RenderResources make_render_resources(const std::shared_ptr<Allocator>& pAllocator)
{
    static constexpr std::uint64_t vertexCapacity = 512 * 128 * 128;    // Aproximately 8,3 million verticies
    VkBufferCreateInfo meshTableInfo{};
    return RenderResources{ .meshTable = pAllocator->create_storage_buffer(meshTableInfo),
                            .vertexBuffer = pAllocator->create_vertex_buffer(vertexCapacity) };
}
}    // namespace
namespace odin::graphics
{
Renderer::Renderer(const std::shared_ptr<vk::Allocator>& pAllocator)
    : m_RenderResources{ make_render_resources(pAllocator) }
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

    // Bind Global buffers

    // Bind descriptors

    // Bind pipelines
    //m_Pipeline.bind(cmdBuffer.handle());


    // Should be handled by the forward pass?
    VkImageMemoryBarrier2 renderBarrier = colorAttachment.barrier_to_render();
    const VkDependencyInfo dep{ .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                                .pNext = nullptr,
                                .dependencyFlags = 0,
                                .memoryBarrierCount = 0,
                                .pMemoryBarriers = nullptr,
                                .bufferMemoryBarrierCount = 0,
                                .pBufferMemoryBarriers = nullptr,
                                .imageMemoryBarrierCount = 1,
                                .pImageMemoryBarriers = std::addressof(renderBarrier) };
    vkCmdPipelineBarrier2(gfxCmdBuffer.handle().handle, std::addressof(dep));


    // Dynamic rendering
    vk::resource::ImageViewRef colorView = colorAttachment.view();
    VkRenderingAttachmentInfo colorAtt{ .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
                                        .pNext = nullptr,
                                        .imageView = colorView.handle,
                                        .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                        .resolveMode = VK_RESOLVE_MODE_NONE,
                                        .resolveImageView = VK_NULL_HANDLE,
                                        .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                                        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,    // or LOAD if you preserved previous
                                        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                                        .clearValue = { .color = { { 1.0f, 0.0f, 1.0f, 1.0f } } } };
    VkRenderingInfo ri{
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .pNext = nullptr,
        .flags = 0,
        .renderArea = { { 0, 0 }, colorAttachment.extent() },
        .layerCount = 1,
        .viewMask = 0,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAtt,
        .pDepthAttachment = nullptr,
        .pStencilAttachment = nullptr
    };
    vkCmdBeginRendering(gfxCmdBuffer.handle().handle, std::addressof(ri));
    vkCmdEndRendering(gfxCmdBuffer.handle().handle);

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
const RenderResources& Renderer::render_resources() const
{
    return m_RenderResources;
}
}    // namespace odin::graphics
